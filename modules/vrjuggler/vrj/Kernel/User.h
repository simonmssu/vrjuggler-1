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
 * -----------------------------------------------------------------
 * File:          $RCSfile$
 * Date modified: $Date$
 * Version:       $Revision$
 * -----------------------------------------------------------------
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#ifndef _VRJ_USER_H_
#define _VRJ_USER_H_

#include <vrj/vrjConfig.h>
#include <jccl/Config/ConfigElementPtr.h>
#include <gadget/Type/PositionInterface.h>
#include <vpr/Util/Interval.h>


namespace vrj
{

/**
 * Representation for Juggler user in multi-user environments.
 *
 * Each user has a system assigned id as well as a string name from the 'user'
 * config element the created the user.  These ids can be used to identify a
 * user at set points in a VR Juggler application where user information is
 * is given.
 *
 * The system-assigned ID number can also be useful to use as an index into a
 * program array (or other data structure) that stores user-specific data
 * (for example, nativgation matrices or input devices).
 */
class VJ_CLASS_API User
{
public:
   /** Constructs the user. */
   User()
      : mUserId(-1)
      , mName("")
      , mInterocularDist(0.0f)
   {;}

   virtual ~User()
   {;}

   /**
    * Returns the ID of the user in the system.
    * @post -1 => User has not been configured.
    */
   int getId();

   /**
    * Returns the name of the user object.
    * @post String name of the head.
    */
   std::string getName();

   /**
    * Configures the user object.
    * @post User has valid ids (int and string).
    * @posr Positional device for user location has been set.
    */
   virtual bool config(jccl::ConfigElementPtr element);

   gadget::PositionProxy* getHeadPosProxy()
   {
      return mHead.getProxy();
   }

   vpr::Interval getHeadUpdateTime()
   {
      return mHead->getTimeStamp();
   }

   float getInterocularDistance()
   {
      return mInterocularDist;
   }

private:
   int               mUserId;           /**< The ID of the user. */
   std::string       mName;             /**< The string name of the user. */
   gadget::PositionInterface    mHead;  /**< The head positon. */
   float             mInterocularDist;  /**< Eye seperation */

private:
   static int mNextUserId;     /**< the next user id to assign */

   User(const User& u) {;}
   void operator=(const User& u) {;}
};

}

#endif
