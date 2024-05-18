#include "stdafx.h"
#include "QueueURI.h"


QueueURI::QueueURI()
{
	pHead = pTail = pSeparate = NULL;
	f_Se = true;
	sizeQueue = 0;
}

QueueURI::~QueueURI()
{
	while (this->pHead)
	{
		NodeQueue* p = this->pHead;
		this->pHead = this->pHead->pNext;
		delete p;
		p = NULL;
	}
}

bool QueueURI::isEmpty()
{
	return (this->pHead == NULL);
}

bool QueueURI::enQueue(URI a_URI)
{
	NodeQueue* newNode = new NodeQueue(a_URI);
	if (newNode == NULL)
	{
		return false;
	}
	// Chèn vào cuối hàng
	if (this->pTail == NULL)
	{
		this->pHead = this->pTail = this->pSeparate = newNode;
	}
	else
	{
		if (this->pSeparate == NULL)		// chèn vào lượt 2
		{
			newNode->pNext = pHead;
			pHead = newNode;
		}
		else
		{
			newNode->pNext = this->pSeparate->pNext;
			this->pSeparate->pNext = newNode;
		}
		this->pSeparate = newNode;
		if (f_Se == true)
		{
			this->pTail = newNode;
		}
	}
	sizeQueue++;
	return true;
}

bool QueueURI::deQueue(URI& a_URI)
{
	// Lấy ra ở đầu hàng
	if (this->isEmpty()) return false;
	sizeQueue--;
	a_URI = this->pHead->data;
	NodeQueue* p = this->pHead;
	this->pHead = this->pHead->pNext;
	if (this->pHead == NULL)
	{
		this->pTail = NULL;
	}
	delete p;
	p = NULL;
	return true;
}