/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998, 1999, 2000 by Iowa State University
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

#include <vector>
#include <iostream>
#include <stdio.h>

#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <VPR/SharedMem/vjMemPool.h>
#include <VPR/Threads/vjThreadPool.h>
#include <Utils/vjDebug.h>
#include <VPR/Sync/vjMutex.h>
#include <VPR/vjSystem.h>

void doIt(void*);

template<class T>
void dumpVector(std::vector<T>& theVector);


const int NUMTHREADS = 2;

std::vector<int> intVector;
    
int main(void)
{
    vjSharedPool myPool(65536, 16);    // size, num threads
    vjThreadPool* thePool = new(&myPool) vjThreadPool(NUMTHREADS);
    vjMutex DebugLock;
    
    std::cout << "\nMax: " << intVector.max_size() << std::endl;

    int params[1000];
    for(int z=0;z<1000;z++)
	params[z] = z;
    
//    for(int j=0;j<10;j++)
//    {
//	for(int x=0;x<5000;x++)
//	    intVector.push_back(x);
//	cerr << intVector.size() << endl;
//    }
	
//-------------------------------------------------
	    // Timing variables
    struct timeval startTime, endTime;		    // Used to calculate time
    double startSecond, endSecond, totalSeconds;    // performance
    
    gettimeofday(&startTime, NULL);	    // Get the starting time
//--------------------------------------------------
    
///*
    for (int i=0;i<15;i++) {
	thePool->startFunc((THREAD_FUNC)doIt, (void*)&(params[i]));      
    }
    thePool->barrier();
//*/
//--------------------------------------------------------
    gettimeofday(&endTime, NULL);	    // Get ending time
    startSecond = (startTime.tv_sec + (startTime.tv_usec * 0.000001));
    endSecond = endTime.tv_sec + (endTime.tv_usec * 0.000001);
    totalSeconds = (endSecond - startSecond);
    
///    std::cout << "\nDone: It took... " << totalSeconds << " seconds" << std::flush;
//---------------------------------------------------------

    vjSystem::usleep(100);
    
    DebugLock.acquire();
	dumpVector(intVector);
    DebugLock.release();    

return 1;
}

void doIt(void* param)
{
    int* x = (int*)param;
    
    for(int i=0;i<100;i++)
	intVector.push_back(i+((*x)*100));    
}

template<class T>
void dumpVector(std::vector<T>& theVector)
{
    for(std::vector<T>::iterator i = theVector.begin(); i != theVector.end(); i++)
	std::cout << "\n" << *i;
    std::cout << std::endl;
}