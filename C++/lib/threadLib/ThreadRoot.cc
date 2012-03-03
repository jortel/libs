///////////////////////////////////////////////////////////////////////
//
//  File/Class: ThreadRoot
// Description: The Thread root object.
//
// Author: Jeff Ortel, 04/05/2001 (Cyberwerx, Inc)
// Modifications:
//
///////////////////////////////////////////////////////////////////////

#include "threadLib/ThreadRoot.h"
#include "threadLib/Thread.h"

ThreadRoot::ThreadRoot()
 :thread(0)
{
}

ThreadRoot::~ThreadRoot()
{
}

void ThreadRoot::operator()(void*)
{
}
