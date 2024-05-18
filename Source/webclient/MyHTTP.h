#pragma once
#include <sys/types.h>
#include <winsock2.h>
#include <direct.h>
#include <fstream>
#include <cmath>
#include "QueueURI.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Class lưu trữ các phương thức để giao tiếp với phương webservice thông qua phương thức HTTP
class MyHTTP
{
private:
	URI m_URI;				// Lưu link để truy cập
	int m_Port;				// Lưu cổng port giao tiếp (webservices 80)
	sockaddr_in m_Addr;
	int m_SocketID;			// SocketID	
	hostent* m_HostEntry;		// Quản lý tên và địa chỉ IP

								// Khởi tạo đối tượng hostent
	hostent* setHostEntry();

	// Lấy ra các link item con trong thư mục
	int getURIFromFolder(char* data, int index, int pSize, char* link, char* parentFolder);

public:
	// Constructor, Destructor
	MyHTTP();
	MyHTTP(URI uri, int port);
	~MyHTTP();

	// Gán URI
	void setURI(URI uri);

	// Thực hiện tạo REQUEST theo version của HTTP

	void makeRequest(char request[1024], const int& type);

	// Khởi tạo kết nối socket
	int Create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);

	// Kết nối tới server
	int Connect();

	// Gửi yêu cầu REQUEST tới server
	void Send(char request[1024]);

	// Nhận dữ liệu từ Server nBytes 

	int Receive(char* pData, int nBytes);

	// Đóng kết nối
	void Close();

	// Lấy header của phần dữ liệu server gửi về, trả về số byte của header
	int GetHeader(char* header, int pSize);

	// Kiểm tra xem header nhận có OK không? nếu OK rồi thì mới lấy dữ liệu
	bool isValidResponse(char* data, const int& versionHTTP);

	// Kiểm tra xem gói header gó bị mã hóa chunk không?
	bool hasChunkedEncoding(char* data);

	// Thực hiện giải mã gói dữ liệu bị chunked encoding, nguồn: src và dữ liệu đã decode vào datam với
	// totalSize là tổng số byte của src
	void decodeChunk(char* data, char* src, int totalSize);

	// Lưu trữ dữ liệu nhận về vào máy (lưu folder và file)
	void writeData(QueueURI& uris, const int& versionHTTP, const bool& hasChunked);
};