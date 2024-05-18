#pragma once
#include "NodeQueue.h"

// Cấu trúc hàng đợi queue lưu trữ các link
class QueueURI
{
public:
	NodeQueue* pHead;
	NodeQueue* pTail;
	NodeQueue* pSeparate;
	bool f_Se;		// Biến cờ dùng đề chèn theo thư mục
	int sizeQueue;
	QueueURI();
	~QueueURI();

	bool isEmpty();
	bool enQueue(URI a_URI);
	bool deQueue(URI& a_URI);
};