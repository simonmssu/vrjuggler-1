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
 * VR Juggler is (C) Copyright 1998, 1999, 2000, 2001 by Iowa State University
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

#ifndef _VPR_MCAST_REQ_H_
#define _VPR_MCAST_REQ_H_

#include <vpr/vprConfig.h>
#include <vpr/IO/Socket/InetAddr.h>


namespace vpr {

/**
 * Multicast request wrapper.
 */
class VPR_CLASS_API McastReq {
public:
    /**
     * Default constructor.
     */
    McastReq (void)
        : mMcastAddr(), mIfAddr()
    {
        /* Do nothing. */ ;
    }

    /**
     *
     */
    McastReq (const InetAddr& mcast_addr, const InetAddr& if_addr)
        : mMcastAddr(mcast_addr), mIfAddr(if_addr)
    {
        /* Do nothing. */ ;
    }

    /**
     * Copy constructor.
     */
    McastReq (const McastReq& mcast_addr) {
        mMcastAddr = mcast_addr.mMcastAddr;
        mIfAddr    = mcast_addr.mIfAddr;
    }

    /**
     *
     */
    inline const InetAddr&
    getMulticastAddr (void) const {
        return mMcastAddr;
    }

    /**
     *
     */
    inline const InetAddr&
    getInterfaceAddr (void) const {
        return mIfAddr;
    }

private:
    InetAddr mMcastAddr;
    InetAddr mIfAddr;
};

}; // End of vpr namespace


#endif  /* _VPR_MCAST_REQ_H_ */
