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
 * VR Juggler is (C) Copyright 1998-2002 by Iowa State University
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

#include <vpr/Util/Assert.h>
#include <vpr/Thread/Thread.h>
#include <vpr/md/NSPR/NSPRHelpers.h>
#include <vpr/md/NSPR/Thread/ThreadNSPR.h>


namespace vpr
{

ThreadTable<PRThread*> ThreadNSPR::mThreadTable;
PRUint32 ThreadNSPR::mTicksPerSec = PR_TicksPerSecond();

// Non-spawning constructor.  This will not start a thread.
ThreadNSPR::ThreadNSPR(VPRThreadPriority priority, VPRThreadScope scope,
                       VPRThreadState state, PRUint32 stackSize)
   : mThread(NULL), mUserThreadFunctor(NULL), mPriority(priority),
     mScope(scope), mState(state), mStackSize(stackSize)
{
}

// Spawning constructor with arguments.  This will start a new thread that
// will execute the specified function.
ThreadNSPR::ThreadNSPR(thread_func_t func, void* arg,
                       VPRThreadPriority priority, VPRThreadScope scope,
                       VPRThreadState state, PRUint32 stackSize)
   : mThread(NULL), mUserThreadFunctor(NULL), mPriority(priority),
     mScope(scope), mState(state), mStackSize(stackSize)
{
   // XXX: Memory leak.
   setFunctor(new ThreadNonMemberFunctor(func, arg));
   start();
}

// Spawning constructor (functor version).  This will start a new thread that
// will execute the specified function.
ThreadNSPR::ThreadNSPR(BaseThreadFunctor* functorPtr,
                       VPRThreadPriority priority, VPRThreadScope scope,
                       VPRThreadState state, PRUint32 stackSize)
   : mThread(NULL), mUserThreadFunctor(NULL), mPriority(priority),
     mScope(scope), mState(state), mStackSize(stackSize)
{
   setFunctor(functorPtr);
   start();
}

// Destructor.
ThreadNSPR::~ThreadNSPR()
{
   mThreadTable.removeThread(gettid());
}

void ThreadNSPR::setFunctor(BaseThreadFunctor* functorPtr)
{
   vprASSERT(mThread == NULL && "Thread already running");
   vprASSERT(functorPtr->isValid());

   mUserThreadFunctor = functorPtr;
}

// Creates a new thread that will execute mUserFunctorPtr.
vpr::ReturnStatus ThreadNSPR::start()
{
   vpr::ReturnStatus status;

   if ( NULL != mThread )
   {
      vprASSERT(false && "Thread already running");
      status.setCode(vpr::ReturnStatus::Fail);
   }
   else if ( NULL == mUserThreadFunctor )
   {
      vprASSERT(false && "No functor set");
      status.setCode(vpr::ReturnStatus::Fail);
   }
   else
   {
      PRThreadPriority nspr_prio;
      PRThreadScope nspr_scope;
      PRThreadState nspr_state;

      nspr_prio  = vprThreadPriorityToNSPR(mPriority);
      nspr_scope = vprThreadScopeToNSPR(mScope);
      nspr_state = vprThreadStateToNSPR(mState);

      vprASSERT(mUserThreadFunctor->isValid());

      // Store the member functor and create the functor for spawning to our
      // start routine.
      // XXX: Memory leak.
      ThreadMemberFunctor<ThreadNSPR>* start_functor =
            new ThreadMemberFunctor<ThreadNSPR>(this, &ThreadNSPR::startThread,
                                                NULL);
      
      // Finally create the thread.
      mThread = PR_CreateThread(PR_USER_THREAD, vprThreadFunctorFunction,
                                (void*) start_functor, nspr_prio, nspr_scope,
                                nspr_state, (PRUint32) mStackSize);

      // Inform the caller if the thread was not created successfully.
      if ( NULL == mThread )
      {
         ThreadManager::instance()->lock();
         {
            registerThread(false);
         }
         ThreadManager::instance()->unlock();
               
         NSPR_PrintError("vpr::ThreadNSPR::spawn() - Cannot create thread");
         status.setCode(vpr::ReturnStatus::Fail);
      }
      else
      {
         ThreadManager::instance()->lock();      // Lock manager
         {
            registerThread(true);                     // Register success
         }
         ThreadManager::instance()->unlock();
      }
   }

   return status;
}

/**
 * Helper method Called by the spawn routine to start the user thread function.
 *
 * @pre Called ONLY by a new thread
 * @post Do any thread registration necessary
 *       Call the user thread functor
 *
 * @param null_param
 */
void ThreadNSPR::startThread(void* null_param)
{
   // WE are a new thread... yeah!!!!
   // TELL EVERYONE THAT WE LIVE!!!!
   ThreadManager::instance()->lock();      // Lock manager
   {
      mThreadTable.addThread(this, mThread);    // Store local lookup
   }
   ThreadManager::instance()->unlock();

   // --- CALL USER FUNCTOR --- //
   (*mUserThreadFunctor)();
}


// ---------------------------------------------------------------------------
// Set this thread's priority.
//
// PRE: None.
// POST: This thread has its priority set to the specified value.
// ---------------------------------------------------------------------------
int ThreadNSPR::setPrio (VPRThreadPriority prio)
{
   int retval;

   retval = 0;

   if ( prio > 3 )
   {
      retval = -1;
   }
   else
   {
      PR_SetThreadPriority(mThread, vprThreadPriorityToNSPR(prio));
   }

   return retval;
}

// ===========================================================================
// Private methods follow.
// ===========================================================================

// ---------------------------------------------------------------------------
// Check the status of the thread creation in order to determine if this
// thread should be registered in the thread table or not.
// ---------------------------------------------------------------------------
/*
void ThreadNSPR::checkRegister (const int status)
{
   if ( status == 0 )
   {
      registerThread(true);
      mThreadTable.addThread(this, mThread);
   }
   else
   {
      registerThread(false);  // Failed to create
   }
}
*/

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
PRThreadPriority ThreadNSPR::vprThreadPriorityToNSPR (const VPRThreadPriority priority)
{
   PRThreadPriority nspr_prio;

   switch ( priority )
   {
      case VPR_PRIORITY_LOW:
         nspr_prio = PR_PRIORITY_LOW;
         break;
      case VPR_PRIORITY_NORMAL:
         nspr_prio = PR_PRIORITY_NORMAL;
         break;
      case VPR_PRIORITY_HIGH:
         nspr_prio = PR_PRIORITY_HIGH;
         break;
      case VPR_PRIORITY_URGENT:
         nspr_prio = PR_PRIORITY_URGENT;
         break;
   };

   return nspr_prio;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
PRThreadScope ThreadNSPR::vprThreadScopeToNSPR (const VPRThreadScope scope)
{
   PRThreadScope nspr_scope;

   switch ( scope )
   {
      case VPR_LOCAL_THREAD:
         nspr_scope = PR_LOCAL_THREAD;
         break;
      case VPR_GLOBAL_THREAD:
         nspr_scope = PR_GLOBAL_THREAD;
         break;
   };

   return nspr_scope;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
PRThreadState ThreadNSPR::vprThreadStateToNSPR (const VPRThreadState state)
{
   PRThreadState nspr_state;

   switch ( state )
   {
      case VPR_JOINABLE_THREAD:
         nspr_state = PR_JOINABLE_THREAD;
         break;
      case VPR_UNJOINABLE_THREAD:
         nspr_state = PR_UNJOINABLE_THREAD;
         break;
   };

   return nspr_state;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
BaseThread::VPRThreadPriority ThreadNSPR::nsprThreadPriorityToVPR (const PRThreadPriority priority)
{
   VPRThreadPriority vpr_prio;

   switch ( priority )
   {
      case PR_PRIORITY_LOW:
         vpr_prio = VPR_PRIORITY_LOW;
         break;
      case PR_PRIORITY_NORMAL:
         vpr_prio = VPR_PRIORITY_NORMAL;
         break;
      case PR_PRIORITY_HIGH:
         vpr_prio = VPR_PRIORITY_HIGH;
         break;
      case PR_PRIORITY_URGENT:
         vpr_prio = VPR_PRIORITY_URGENT;
         break;
   };

   return vpr_prio;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
BaseThread::VPRThreadScope ThreadNSPR::nsprThreadScopeToVPR (const PRThreadScope scope)
{
   VPRThreadScope vpr_scope;

   switch ( scope )
   {
      case PR_LOCAL_THREAD:
         vpr_scope = VPR_LOCAL_THREAD;
         break;
      case PR_GLOBAL_THREAD:
         vpr_scope = VPR_GLOBAL_THREAD;
         break;
   };

   return vpr_scope;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
BaseThread::VPRThreadState ThreadNSPR::nsprThreadStateToVPR (const PRThreadState state)
{
   VPRThreadState vpr_state;

   switch ( state )
   {
      case PR_JOINABLE_THREAD:
         vpr_state = VPR_JOINABLE_THREAD;
         break;
      case PR_UNJOINABLE_THREAD:
         vpr_state = VPR_UNJOINABLE_THREAD;
         break;
   };

   return vpr_state;
}

} // End of vpr namespace
