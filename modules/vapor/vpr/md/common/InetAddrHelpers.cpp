/****************** <VPR heading BEGIN do not edit this line> *****************
 *
 * VR Juggler Portable Runtime
 *
 * Original Authors:
 *   Allen Bierbaum, Patrick Hartling, Kevin Meinert, Carolina Cruz-Neira
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile$
 * Date modified: $Date$
 * Version:       $Revision$
 * -----------------------------------------------------------------
 *
 ****************** <VPR heading END do not edit this line> ******************/

/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2005 by Iowa State University
 *
 * Original Authors:
 *   Allen Bierbaum, Christopher Just,
 *   Patrick Hartling, Kevin Meinert,
 *   Carolina Cruz-Neira, Albert Baker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#include <vpr/vprConfig.h>

#include <vector>

#if defined(VPR_OS_Windows)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>

#  if defined(HAVE_GETIFADDRS)
#     include <ifaddrs.h>
#  else
#     include <sys/ioctl.h>
#     include <net/if.h>
#  endif

#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <errno.h>
#endif

#include <vpr/IO/Socket/InetAddr.h>
#include <vpr/Util/Debug.h>
#include <vpr/Util/ReturnStatus.h>


namespace vpr
{

/**
 * Retrieves all the IPv4 addresses associated with the local machine,
 * including the loopback address (127.0.0.1) if so indicated. This is an
 * internal function that is not part of the public VPR interface.
 *
 * This function exists in this form primarily because NSPR does not provide
 * any wrapper that offerrs this functionality. Since the WinSock2 use of
 * WSAIoctl() so closely mimics the use of ioctl(2) to get the interface
 * addresses, much code duplication would be required between vpr::InetAddrBSD
 * and vpr::InetAddrNSPR. Instead of duplicating complicated code, those
 * classes call into this function.
 *
 * @post \p hostAddrs contains vpr::InetAddr objetcs holding all the local
 *       IPv4 addresses for the local machine.
 *
 * @param hostAddrs    Storage for the discovered local IPv4 addresses. The
 *                     vector is cleared before the addresses are added, so
 *                     any objects currently in the vector are lost.
 * @param withLoopback A flag indicating whether to include the loopback
 *                     address (127.0.0.1) in \p hostAddrs.
 *
 * @note This method currently supports only IPv4.
 *
 * @return vpr::ReturnStatus::Succeed is returned if the querying of all local
 *         addresses completes without error. Otherwise,
 *         vpr::ReturnStatus::Fail is returned.
 */
vpr::ReturnStatus getIfAddrs(std::vector<vpr::InetAddr>& hostAddrs,
                             const bool withLoopback)
{
#if defined(VPR_OS_Windows)
   const unsigned long loop = ntohl(INADDR_LOOPBACK);
#else
   const in_addr_t loop = ntohl(INADDR_LOOPBACK);
#endif

   // Make sure hostAddrs is empty so that we can use push_back() below.
   hostAddrs.clear();

#if defined(HAVE_GETIFADDRS)
   ifaddrs* addrs(NULL);

   int result = getifaddrs(&addrs);

   if ( result < 0 )
   {
      vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
         << clrOutBOLD(clrRED, "ERROR")
         << ": [vpr::getIfAddrs()] Failed to query interface addresses: "
         << strerror(errno) << std::endl << vprDEBUG_FLUSH;
      return vpr::ReturnStatus::Fail;
   }
   else
   {
      for ( ifaddrs* a = addrs; a != NULL; a = a->ifa_next )
      {
         sockaddr_in* addr_in = (sockaddr_in*) a->ifa_addr;

         // We only handle IPv4 addresses.
         if ( addr_in->sin_family != AF_INET )
         {
            continue;
         }

         // If we have the loopback address and withLoopback is false, then we
         // skip this address.
         if ( addr_in->sin_addr.s_addr == loop && ! withLoopback )
         {
            continue;
         }

         char netaddr[18];
         const char* temp_addr = inet_ntop(addr_in->sin_family,
                                           &addr_in->sin_addr, netaddr,
                                           sizeof(netaddr));

         if ( NULL != temp_addr )
         {
            vpr::InetAddr vpr_addr;
            vpr_addr.setAddress(netaddr, 0);
            hostAddrs.push_back(vpr_addr);
         }
      }
   }

   freeifaddrs(addrs);
#else   /* ! HAVE_GETIFADDRS */
   // While the implementation of this method is long and rather complex
   // looking, the idea is simple:
   //
   //    1. Create an IPv4 socket.
   //    2. Use ioctl(2) or WSAIoctl() on the socket handle to query all the
   //       network interfaces.
   //    3. Iterate over the returned interface request objects and extract
   //       the valid IPv4 addresses.
   //    4. Store each IPv4 address in a new vpr::InetAddr object.
#if defined(VPR_OS_Windows)
   typedef INTERFACE_INFO ifreq_t;

   SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);

   // Socket creation failed, so we cannot proceed.
   if ( sock == SOCKET_ERROR )
   {
      vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
         << clrOutBOLD(clrRED, "ERROR")
         << ": [vpr::getIfAddrs()] Socket creation failed (error code "
         << WSAGetLastError() << ")\n" << vprDEBUG_FLUSH;
      return vpr::ReturnStatus::Fail;
   }
#else
   typedef ifreq ifreq_t;

   int sock = socket(AF_INET, SOCK_DGRAM, 0);

   // Socket creation failed, so we cannot proceed.
   if ( sock < 0 )
   {
      vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
         << clrOutBOLD(clrRED, "ERROR")
         << ": [vpr::getIfAddrs()] Socket creation failed: " << strerror(errno)
         << std::endl << vprDEBUG_FLUSH;
      return vpr::ReturnStatus::Fail;
   }
#endif

   // Initial guess on the size of the buffer that will be returned by
   // ioctl(2) or WSAIoctl().
   unsigned int num_reqs(32);

   size_t lastlen(0);
   unsigned long bytes_returned(0);
#if ! defined(VPR_OS_Windows)
   ifconf ifc;
#endif
   ifreq_t* if_list(NULL);

   // Call ioctl(2) or WSAIoctl() iteratively to ensure that we get back all
   // the interface addresses. This is based on code from NSPR.
   for ( ; ; )
   {
      // Allocate storage for the data returned by ioctl(2) or WSAIoctl().
      if_list = new ifreq_t[num_reqs];

      if ( NULL == if_list )
      {
#if defined(VPR_OS_Windows)
         closesocket(sock);
#else
         close(sock);
#endif
         vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
            << clrOutBOLD(clrRED, "ERROR")
            << ": [vpr::getIfAddrs()] Out of memory" << std::endl
            << vprDEBUG_FLUSH;
         return vpr::ReturnStatus::Fail;
      }

      const size_t len = sizeof(ifreq_t) * num_reqs;
#if defined(VPR_OS_Windows)
      int result = WSAIoctl(sock, SIO_GET_INTERFACE_LIST, NULL, 0, if_list,
                            len, &bytes_returned, NULL, NULL);
#else
      ifc.ifc_len = len;
      ifc.ifc_req = if_list;

      int result = ioctl(sock, SIOCGIFCONF, &ifc);
      bytes_returned = ifc.ifc_len;
#endif

      // Ask the kernel for all the network interfaces to which sock could be
      // bound.
#if defined(VPR_OS_Windows)
      if ( result == SOCKET_ERROR )
#else
      if ( result < 0 )
#endif
      {
#if defined(VPR_OS_Windows)
         const int inval_err(WSAEINVAL);
         const int err_code(WSAGetLastError());
#else
         const int inval_err(EINVAL);
         const int err_code(errno);
#endif

         // If ioctl(2) or WSAIoctl() failed for reasons other than our buffer
         // being too small, then we cannot continue. We need to clean up after
         // ourselves before we return failure status.
         if ( err_code != inval_err || lastlen != 0 )
         {
#if defined(VPR_OS_Windows)
            closesocket(sock);
#else
            close(sock);
#endif
            delete[] if_list;

            vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
               << clrOutBOLD(clrRED, "ERROR") << ": [vpr::getIfAddrs()] "
#if defined(VPR_OS_Windows)
               << "Bad ioctl (error code " << err_code << ")"
#else
               << "Bad ioctl: " << strerror(errno)
#endif
               << std::endl << vprDEBUG_FLUSH;
            return vpr::ReturnStatus::Fail;
         }
      }
      // If ioctl(2) or WSAIoctl() returned successfully, check the size of
      // the buffer that it returned.
      else
      {
         // If lastlen is the same as the size of the buffer returned by
         // ioctl(2) or WSAIoctl(), then we are done. If the size of the buffer
         // returned by ioctl(2) or WASIoctl() is no larger than what we
         // allocated, then we are done.
         if ( bytes_returned == lastlen || bytes_returned <= len )
         {
            break;
         }
         // Otherwise, we continue iterating.
         else
         {
            lastlen = bytes_returned;
         }
      }

      // Increment the size of the returned buffer by increasing the number of
      // allocated request structures.
      num_reqs += 10;
      delete[] if_list;
   }

   // We are done with the socket.
#if defined(VPR_OS_Windows)
   closesocket(sock);
#else
   close(sock);
#endif

   // Figure out how many interfaces were returned.
   const size_t num = bytes_returned / sizeof(ifreq_t);

   // Iterate over the returned ifreq objects and pull out the valid IPv4
   // addresses.
   for ( size_t i = 0; i < num; ++i )
   {
#if defined(VPR_OS_Windows)
      sockaddr_in* addr = (sockaddr_in*) &if_list[i].iiAddress;
#else
      sockaddr_in* addr = (sockaddr_in*) &ifc.ifc_req[i].ifr_addr;
#endif

      // Skip addresses that are not IPv4.
      // XXX: We should support IPv6 at some point.
      if ( addr->sin_family != AF_INET )
      {
         continue;
      }

      // If we have the loopback address and withLoopback is false, then we
      // skip this address.
      if ( addr->sin_addr.s_addr == loop && ! withLoopback )
      {
         continue;
      }

      char netaddr[18];
#if defined(VPR_OS_Windows)
      // inet_ntoa() returns a pointer to static memory, which means that
      // it is not reentrant. There is a race condition here between the
      // time of the return from inet_ntoa() and the time that strcpy()
      // actually copies the memory. Unfortunately, WinSock2 does not
      // provide inet_ntop().
      strcpy(netaddr, inet_ntoa(addr->sin_addr));
      char* temp_addr = netaddr;
#else
      const char* temp_addr = inet_ntop(addr->sin_family, &addr->sin_addr,
                                        netaddr, sizeof(netaddr));
#endif

      if ( NULL != temp_addr )
      {
         vpr::InetAddr vpr_addr;
         vpr_addr.setAddress(netaddr, 0);
         hostAddrs.push_back(vpr_addr);
      }
   }

   // All done.
   delete[] if_list;
#endif  /* defined(HAVE_GETIFADDRS) */

   return vpr::ReturnStatus::Succeed;
}

}
