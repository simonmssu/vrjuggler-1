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

//=====================================================
// Vrpn
//
// Purpose:
//    VR Juggler VRPN Hi-Ball tracking class
//
// Author:
//    Jason Jerald
// Last Modified:  8-26-02
//
// Revisions:
//      Ported to to 1.1 DR2 and sgi platform Anthony Steed, 10-4-02
//======================================================

#ifndef _GADGET_VRPN_H_
#define _GADGET_VRPN_H_

#include <gadget/Devices/DriverConfig.h>
#include <vector>
#include <gadget/Type/Input.h>
#include <gadget/Type/Position.h>
#include <gadget/Type/Digital.h>
#include <gadget/Type/InputMixer.h>

#include <gmtl/Matrix.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Vec.h>
#include <gmtl/Quat.h>

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>

namespace gadget
{
   class InputManager;
}

extern "C" GADGET_DRIVER_API(void) initDevice(gadget::InputManager* inputMgr);

namespace gadget
{

   /**
    * Class interfacing with vrpn sensor data located on the local machine in
    * a shared memory arena.
    *
    * @note A note on reciever access:
    *  Clients of Juggler should access tracker recievers as [0-n].  For
    *  example, if you have recievers 1,2, and 4 with transmitter on 3, then
    *  you can access the data, in order, as 0, 1, 2.
    *
    * @see gadget::Digital, gadget::Analog, gadget::Position
    */
   class Vrpn : public InputMixer<InputMixer<Input,Digital>,Position>
   {
    
   public:
    
      /** Constructor. */
      Vrpn():mReadThread(NULL){};
    
      /**
       * Destructor.
       *
       * @pre None.
       * @post Shared memory is released.
       */
      ~Vrpn();

      /**
       * Configures the VRPN with the given config chunk.
       *
       * @pre c must be a chunk that has vrpn  config information
       * @post If c is a valid config chunk, the device is configured using its
       *       contents.  Otherwise, configuration fails and false is returned
       *       to the caller.
       *
       * @param c A pointer to a config chunk.
       *
       * @return true is returned if the device was configured succesfully.
       *         false is returned if the config chunk is invalid.
       */
      virtual bool config(jccl::ConfigChunkPtr c);

      /** Begins sampling. */
      int startSampling();

      /** Stops sampling. */
      int stopSampling();

      /** Samples a value. */
      int sample();

      /**
       * Update to the sampled data.
       *
       * @pre None.
       * @post Most recent value is copied over to temp area.
       */
      void updateData();

      /** Returns what chunk type is associated with this class. */
      static std::string getChunkType()
      {
         return std::string("Vrpn");
      }

      /**
       * Invokes the global scope delete operator.  This is required for proper
       * releasing of memory in DLLs on Win32.
       */
      void operator delete(void* p)
      {
         ::operator delete(p);
      }

   protected:
      /**
       * Deletes this object.  This is an implementation of the pure virtual
       * gadget::Input::destroy() method.
       */
      virtual void destroy()
      {
         delete this;
      }

   private:
      vpr::Thread* mReadThread;
      std::string mTrackerServer;
      std::string mButtonServer;
      int mTrackerNumber;
      int mButtonNumber;
   
      void handleTracker(vrpn_TRACKERCB t);
      void handleButton(vrpn_BUTTONCB b);

      void setPreTransform(float xoff, float yoff, float zoff,  
                           float xrot, float yrot, float zrot,
                           float xscale, float yscale, float zscale);
      void setPostTransform(float xscale, float yscale, float zscale);

      void readLoop(void *nullParam);


      gmtl::Matrix44f getSensorPos(int d);
      gadget::DigitalData getDigitalData(int d);
     
      gmtl::Matrix44f   mPreMatrixTransform;
      gmtl::Matrix44f   mPostMatrixTransform;
      std::vector<DigitalData>  mCurButtons; /**< The current button states. */
      std::vector<PositionData> mCurPositions; /**< The current button states. */

      // Working space - AJS to remove
      std::vector<gadget::DigitalData> mButtons;
      std::vector<gmtl::Quatf> mQuats;
      std::vector<gmtl::Vec3f> mPositions; 

      friend void staticHandleTracker(void *userdata, vrpn_TRACKERCB t);
      friend void staticHandleButton(void *userdata, vrpn_BUTTONCB t);
   };

} // End of gadget namespace


#endif