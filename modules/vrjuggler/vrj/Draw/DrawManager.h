/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2011 by Iowa State University
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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#ifndef _VRJ_DRAW_MANAGER_H_
#define _VRJ_DRAW_MANAGER_H_

#include <vrj/vrjConfig.h>
#include <iostream>
#include <jccl/RTRC/ConfigElementHandler.h>
#include <vrj/Display/DisplayPtr.h>


namespace vrj
{

class DisplayManager;
class App;

/** \class DrawManager DrawManager.h vrj/Draw/DrawManager.h
 *
 * Abstract base class for API-specific Draw Manager.
 *
 * Concrete classes are responsible for all rendering.
 *
 * @date September 7, 1997
 */
class VJ_API DrawManager
   : public jccl::ConfigElementHandler
{
public:
   DrawManager()
      : mDisplayManager(NULL)
   {
      /* Do nothing. */ ;
   }

   virtual ~DrawManager()
   {
      /* Do nothing. */ ;
   }

   /**
    * Function to initialy config API-specific stuff.
    * Takes a jccl::Configuration and extracts API-specific stuff.
    */
   //**//virtual void configInitial(jccl::Configuration*  cfg) = 0;

   /// Enable a frame to be drawn
   virtual void draw() = 0;

   /**
    * Blocks until the end of the frame.
    * @post The frame has been drawn.
    */
   virtual void sync() = 0;

   /**
    * Sets the application with which the Draw Manager will interact.
    *
    * @note The member variable is not in the base class because its "real"
    *       type is only known in the derived classes.
    */
   virtual void setApp(App* app) = 0;

   /**
    * Initializes the drawing API (if not already running).
    *
    * @note If the draw manager should be an active object, start the process
    *       here.
    */
   virtual void initAPI() = 0;

   /** Callback invoked when display is added to the Display Manager. */
   virtual void addDisplay(DisplayPtr disp) = 0;

   /** Callback invoked when display is removed from the Display Manager. */
   virtual void removeDisplay(DisplayPtr disp) = 0;

   /**
    * Shuts down the drawing API.
    *
    * @note If it was an active object, kill process here.
    */
   virtual void closeAPI() = 0;

   /** Setter for Display Manager variable. */
   void setDisplayManager(DisplayManager* dispMgr);
   DisplayManager* getDisplayManager();

   virtual void outStream(std::ostream& out) const;

protected:
   friend VJ_API std::ostream& operator<<(std::ostream& out,
                                          const DrawManager& drawMgr);

   DisplayManager* mDisplayManager;  /**< The display manager dealing with */
};

} // End of vrj namespace


#endif
