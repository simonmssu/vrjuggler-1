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
 * VR Juggler is (C) Copyright 1998-2003 by Iowa State University
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

#ifndef _VPR_SYSTEM_BASE_H_
#define _VPR_SYSTEM_BASE_H_

#include <vpr/vprConfig.h>

#if defined(VPR_OS_FreeBSD) || defined(VPR_OS_Linux)
#include <sys/types.h>
#include <unistd.h>
#include <execinfo.h>

#include <sstream>
#endif



namespace vpr
{

/**
 * vpr::SystemBase is a base class for vpr::System so, for xplatform system
 * functions, use vpr::System (don't use vpr::SystemBase).
 *
 * @see vpr::SystemPosix
 * @see vpr::SystemNSPR
 */
class VPR_CLASS_API SystemBase
{
public:
   /**
    * Determines the endianness of the host platform.  A return nvalue of 0
    * means that the host uses little-endian byte order.  A return value of
    * 1 means that the host uses big-endian byte order.
    *
    * @return 0 is returned for little-endian hosts<br>
    *         1 is returned for big-endian hosts
    */
   static int getEndian (void)
   {
      union
      {
         char   c[sizeof(short)];
         short  value;
      } endian;

      // The way this works is that we access the first byte of endian.value
      // directly.  If it is 1, the host treats that as the high-order byte.
      // Otherwise, it is the low-order byte.
      endian.value = 256;

      return endian.c[0];
   }

   /**
    * Tells if the host uses little-endian byte order or not.
    *
    * @return <code>true</code> is returned on a little-endian host.<br>
    *         <code>false</code> is returned on a big-endian host.<br>
    */
   static bool isLittleEndian (void)
   {
      return(getEndian() == 0);
   }

   /**
    * Tells if the host uses big-endian byte order or not.
    *
    * @return <code>true</code> is returned on a big-endian host.<br>
    *         <code>false</code> is returned on a little-endian host.<br>
    */
   static bool isBigEndian (void)
   {
      return(getEndian() == 1);
   }

   /** Return a stack trace.
   * @post If supported, returns a string describing the current call stack
   */
   static std::string getCallStack()
   {
      std::string ret_stack("<Call stack not supported>");

// XXX: should come up with better test for glib
#if defined(VPR_OS_FreeBSD) || defined(VPR_OS_Linux)
      void* trace_syms[100];
      size_t size;
      char **strings;

      pid_t cur_pid = getpid();
      size = backtrace (trace_syms, 100);
      strings = backtrace_symbols (trace_syms, size);

      std::ostringstream trace_stream;
      trace_stream << "Stack trace: thread: " << cur_pid << std::endl;

      for (size_t i = 0; i < size; i++)
      {  trace_stream << "   " << strings[i] << std::endl;  }

      free (strings);

      ret_stack = trace_stream.str();
#endif

      return ret_stack;
   }
};

} // End of vpr namespace


#endif   /* _VPR_SYSTEM_BASE_H_ */
