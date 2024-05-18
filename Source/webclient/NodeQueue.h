#pragma once
#include "URI.h"
// Lưu node của hàng đời, dữ liệu của mỗi node là 1 URI
class NodeQueue
{
public:
	URI data;
	NodeQueue* pNext;
	NodeQueue();
	NodeQueue(URI a_URI);
	~NodeQueue();
};

