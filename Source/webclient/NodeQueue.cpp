#include "stdafx.h"
#include "NodeQueue.h"


NodeQueue::NodeQueue()
{
	pNext = NULL;
}

NodeQueue::NodeQueue(URI a_URI)
{
	this->data = a_URI;
	pNext = NULL;
}

NodeQueue::~NodeQueue()
{
}