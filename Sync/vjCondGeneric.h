/*
 *  File:          $RCSfile$
 *  Date modified: $Date$
 *  Version:       $Revision$
 *
 *
 *                                VR Juggler
 *                                    by
 *                              Allen Bierbaum
 *                             Christopher Just
 *                             Patrick Hartling
 *                            Carolina Cruz-Neira
 *                               Albert Baker
 *
 *                  Copyright (C) - 1997, 1998, 1999, 2000
 *              Iowa State University Research Foundation, Inc.
 *                            All Rights Reserved
 */


#ifndef _vjCondGeneric_h_
#define _vjCondGeneric_h_

#include <vjConfig.h>

#include <sys/types.h>
//#include <unistd.h>

#include <Sync/vjSemaphore.h>
#include <Sync/vjMutex.h>
#include <Kernel/vjDebug.h>

//----------------------------------------------
//  vjCondGeneric
//
// Purpose:
//:   Condition Variable wrapper for the any system
//
//    Condition variables allow a locked test waiting
//  for specific conditions to be satisfied.  For
//  example waiting for a flag to be set or for a
//  counter to fall below a certain value.
//
// Example:
//    (Insert Example here)
//
// Author:
//	Allen Bierbaum
//
// Date: 1-29-97
//!PUBLIC_API:
//-----------------------------------------------
class  vjCondGeneric
{
public:
   //: Constructor
   //! ARGS: mutex - a pointer to a user specified mutex
   //+               if not specified, uses internal mutex
   vjCondGeneric(vjMutex* mutex = NULL)
   {
      if (mutex == NULL)
         mutex = &defaultMutex;

      condMutex = mutex;
      waiters = 0;

      cerr << "-----------------------------------\n"
           << "  vjCondGeneric: DOES NOT WORK\n"
           << "-----------------------------------\n";
   }

   //: Wait for possible condition change
   //! POST: The condition has been modifed, but may not be satisfied.
   //! NOTE: The call blocks until a condition has been signaled
   int wait(void);

   //: Signal a condition change
   // This call tells all waiters that the condition has changed.
   // They can then check to see if it now sarisfies the condition
   int signal ()
   {
      cerr << setw(5) << getpid() << "  Signal" << endl;
      // ASSERT:  We have been locked
      if (condMutex->test() == 0)    // Not locked
         cerr << " vjCondGeneric::signal: Mutex was not locked when signal called!!!" << endl;

      if (waiters > 0)
      {
         int ret_val = sema.release();
         return ret_val;
      }
      else
         return 0;
   }

   //: Signal all waiting threads
   // This releases all waiting threads.
   int broadcast ()
   {
      // ASSERT:  We have been locked
      if (condMutex->test() == 0)    // Not locked
         cerr << " vjCondGeneric::broadcast: Mutex was not locked when broadcase called!!!" << endl;

      for (int i = waiters;i>0;i--)
         sema.release();

      return 0;
   }

   //: Acquire the condition lock.
   int acquire()
   {
      return condMutex->acquire();
   }

   //: Try to acquire the condition lock.
   int tryAcquire()
   {
      return condMutex->tryAcquire();
   }

   //: Release the condition lock.
   int release()
   {
      return condMutex->release();
   }

   //: Explicitly set the mutex to use.
   //! NOTE: NEVER call except to initialize explicitly.
   void setMutex(vjMutex* mutex)
   {
      mutex->release();       // NOT exactly correct, but just make sure not to leave it locked
      condMutex = mutex;
   }

   void dump (void) const
   {
      vjDEBUG_BEGIN(vjDBG_ALL,0) << "-------------  vjCondGeneric::Dump ---------\n" << vjDEBUG_FLUSH;
      vjDEBUG(vjDBG_ALL,0) << "waiters: " << waiters << endl << vjDEBUG_FLUSH;
      condMutex->dump();
      vjDEBUG_END(vjDBG_ALL,0) << "-----------------------------------\n" << vjDEBUG_FLUSH;
   }


private:
   // --- These make up the "condition variable" ---- ///
   vjSemaphore sema;          // Condition variable.
   long waiters;              //: The number of processes waiting

   vjMutex* condMutex;        //: Mutex for the condition variable - User specified
   vjMutex defaultMutex;      //: Mutex to use if user does not specify one

   // = Prevent assignment and initialization.
   void operator= (const  vjCondGeneric&) {}
   vjCondGeneric (const  vjCondGeneric &c) {;}
};

#endif
