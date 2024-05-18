#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "MyHTTP.h"
#include <WinSock2.h>

MyHTTP::MyHTTP()
{
	m_Port = 0;
	m_SocketID = 0;
	m_HostEntry = NULL;
}

MyHTTP::MyHTTP(URI uri, int port)
{
	this->m_URI = uri;
	this->m_Port = port;
	m_SocketID = 0;
}

MyHTTP::~MyHTTP()
{

}

void MyHTTP::setURI(URI uri)
{
	this->m_URI = uri;
}

hostent* MyHTTP::setHostEntry()
{
	m_HostEntry = gethostbyname(this->m_URI.hostname);
	return m_HostEntry;
}

int MyHTTP::Create(int domain, int type, int protocol)
{
	this->m_SocketID = (int)socket(domain, type, protocol);
	if (this->m_SocketID == 0) return 0;
	if (this->setHostEntry() == NULL) return 0;
	this->m_Addr.sin_family = AF_INET;
	this->m_Addr.sin_addr = *((in_addr*)*this->m_HostEntry->h_addr_list);
	this->m_Addr.sin_port = htons(m_Port); // (unsigned short)
	return this->m_SocketID;
}

int MyHTTP::Connect()
{
	if (connect(this->m_SocketID, (sockaddr*)&this->m_Addr, sizeof(sockaddr_in)) == -1)
		return 0;
	return 1;
}

void MyHTTP::makeRequest(char request[1024], const int& type)
{
	if (type != 0)
		sprintf(request, "GET %s HTTP/1.1\r\n""Host: %s\r\n""Connection: close\r\n""\r\n", 
			this->m_URI.pathname, this->m_URI.hostname);
}

void MyHTTP::Send(char request[1024])
{
	send(this->m_SocketID, request, strlen(request), 0);
}

int MyHTTP::Receive(char* pData, int nBytes)
{
	int m_Bytes = 0;
	m_Bytes = recv(this->m_SocketID, pData, nBytes, 0);
	return m_Bytes;
}

int MyHTTP::GetHeader(char* header, int pSize)
{
	// Lấy dữ liệu gửi về cho tới khi nhận được dấu hiệu kết thúc header
	// \r\n\r\n
	int m_Bytes = 0;
	char letter;
	int i_RN = 0;
	for (m_Bytes; m_Bytes < pSize; m_Bytes++)
	{
		if (recv(this->m_SocketID, &letter, 1, 0) > 0)
		{
			if (letter == '\n')
			{
				i_RN++;
			}
			else if (letter == '\r')
			{
				i_RN++;
			}
			else
			{
				i_RN = 0;
			}
			if (i_RN == 4)
			{
				header[m_Bytes] = 0;
				break;
			}
			header[m_Bytes] = letter;
		}
		else
		{
			break;
		}
	}

	return m_Bytes;

}

bool MyHTTP::isValidResponse(char* data, const int& versionHTTP)
{
	// Chưa cập nhật version http
	if (versionHTTP == 1)
	{
		if (strstr(data, "HTTP/1.1 200 OK"))
			return true;
	}
	else if (versionHTTP == 0)
	{
		if (strstr(data, "HTTP/1.0 200 OK"))
			return true;
		if (strstr(data, "HTTP/1.1 200 OK"))
			return true;
	}

	return false;
}

int MyHTTP::getURIFromFolder(char* data, int index, int pSize, char* link, char* parentFolder)
{
	for (int i = index; i < pSize; i++)
	{
		if (toupper(data[i]) == 'H')
		{
			char c1, c2, c3;
			c1 = toupper(data[i + 1]);
			c2 = toupper(data[i + 2]);
			c3 = toupper(data[i + 3]);
			if (c1 == 'R' && c2 == 'E' && c3 == 'F')
			{
				while (data[i] != '"')
				{
					i++;
				}

				// Lọc các trường hợp không phải subitem con
				if (data[i + 1] == '/' && data[i + 2] == '"')
				{
					continue;
				}
				if (data[i + 1] == '?')
				{
					continue;
				}
				if (data[i + 1] == '.' && data[i + 2] == '.' && data[i + 3] == '/')
				{
					continue;
				}
				if (toupper(data[i + 1]) == 'H')
				{
					char cT2 = toupper(data[i + 2]);
					char cT3 = toupper(data[i + 3]);
					char cP = toupper(data[i + 4]);
					if (cT2 == 'T' && cT3 == 'T' && cP == 'P')
					{
						continue;
					}
				}

				int i_Link = 0;
				for (int j = i + 1; j < pSize; j++)
				{
					if (data[j] == '"')
					{
						break;
					}
					else
					{
						link[i_Link++] = data[j];
					}
				}
				link[i_Link] = 0;

				int strLenLink = strlen(link);
				// Loại bỏ '/' đầu dòng
				if (link[0] == '/')
				{
					for (int i1 = 1; i1 < strLenLink; i1++)
					{
						link[i1 - 1] = link[i1];
					}
					link[strLenLink - 1] = 0;
				}


				if (strstr(parentFolder, link))
				{
					i += strlen(link);
				}
				else
				{
					return i + strlen(link);
				}

			}
		}
	}
	return -1;
}

bool MyHTTP::hasChunkedEncoding(char* data)
{
	if (strstr(data, "Transfer-Encoding: chunked"))
		return true;
	return false;
}

void MyHTTP::decodeChunk(char* data, char* src, int totalSize)
{
	int i_Chunk = 0;
	int i_Data = 0;
	int i_Hex;
	char hex[8];
	while (i_Chunk < totalSize)
	{
		i_Hex = 0;
		strcpy_s(hex, "");
		// Thực hiện việc đọc hexa
		while (src[i_Chunk] != '\r')
		{
			hex[i_Hex] = toupper(src[i_Chunk]);
			i_Chunk++;
			i_Hex++;
		}
		hex[i_Hex] = 0;

		// Chuyển mã hex sang số int để đọc số byte gửi
		int num_hex = 0;
		int strLenHex = strlen(hex);
		for (int i = 0; i < strLenHex; i++)
		{
			if (hex[i] >= '0' && hex[i] <= '9')
			{
				num_hex = num_hex + (hex[i] - 48) * pow(16, strLenHex - i - 1);
			}
			else if (hex[i] >= 'A' && hex[i] <= 'F')
			{
				num_hex = num_hex + (hex[i] - 55) * pow(16, strLenHex - i - 1);
			}
		}
		i_Chunk += 2;

		// Kết thúc chunk
		if (num_hex == 0)
		{
			return;
		}

		// Gán data
		for (int i = 0; i < num_hex; i++)
		{
			data[i_Data++] = src[i_Chunk++];
		}

		i_Chunk += 2;
	}
	data[i_Data] = 0;
}

void MyHTTP::writeData(QueueURI& uris, const int& versionHTTP, const bool& hasChunked)
{
	char data[65535];
	char full_item_name[514];

	// Thực hiện việc lấy tên và đường dẫn tập tin từ đường dẫn gốc ban đầu.
	strcpy_s(full_item_name, "");
	strcat_s(full_item_name, this->m_URI.pre_ItemName);
	if (_strcmpi(full_item_name, "21120396_21120419_21120446_") != 0)
		strcat_s(full_item_name, "/");

	char hostnitemname[1024];
	strcpy_s(hostnitemname, "");
	strcat_s(hostnitemname, this->m_URI.hostname);
	strcat_s(hostnitemname, "_");
	strcat_s(hostnitemname, this->m_URI.itemname);
	
	strcat_s(full_item_name, hostnitemname);
	int length = 0;


	// Nếu là file
	if (this->m_URI.isFile)
	{
		fstream myFile;
		myFile.open(full_item_name, ios::out | ios::binary);
		int file_size = 0;
		if (myFile)
		{
			cout << "*** Downloading file: " << hostnitemname << ": ";
			while ((length = recv(this->m_SocketID, data, sizeof(data), 0)) > 0)
			{
				myFile.write(data, length);
				file_size += length;
			}
			cout << file_size << " bytes" << endl;
			myFile.close();
		}
		myFile.close();
	}
	// Nếu là folder
	else
	{
		// Tạo folder trong máy
		_mkdir(full_item_name);

		cout << "@@ Downloading folder: " << hostnitemname << endl;


		char chunked[65535];
		int i_data_decode = 0;
		bool f_Data_Received = false;
		char buffer[65535];
		strcpy_s(buffer, "");
		// Đầu tiên phải nạp toàn bộ dữ liệu của gói tin về rồi về mới thực hiện
		// decode chunked
		int totalSize = 0;
		while (1)
		{
			length = recv(this->m_SocketID, chunked, sizeof(chunked), 0);
			if (length == 0)
			{
				// Tiến hành decode buffer nếu header có chứa chunked encoding
				if (versionHTTP == 1 && hasChunked == true)
				{
					this->decodeChunk(data, buffer, totalSize);
				}
				break;

			}
			else
			{
				f_Data_Received = true;
				chunked[length] = 0;
				strcat_s(buffer, chunked);
				totalSize += length;
			}
		}

		// -------- Thêm các item con trong folder vào hàng đợi -----------
		if (f_Data_Received)
		{
			char link[256];
			strcpy_s(link, "");
			int i_Data = 0;
			while (i_Data >= 0)
			{
				int icount = 0;

				if (versionHTTP == 1 && hasChunked == true)
				{
					i_Data = this->getURIFromFolder(data, i_Data, strlen(data), link, this->m_URI.url);
				}
				else
				{
					i_Data = this->getURIFromFolder(buffer, i_Data, strlen(buffer), link, this->m_URI.url);
				}

				if (i_Data >= 0)
				{
					char url[1024];
					strcpy_s(url, "");
					strcpy_s(url, this->m_URI.url);
					strcat_s(url, link);

					// Thêm uri vào hàng đợi uris
					URI newURI;
					newURI.setURL(url, full_item_name);
					newURI.separateURL();
					uris.enQueue(newURI);
				}
				else
				{
					strcpy_s(link, "");
				}
			}
		}

		// Cho uri vào hàng đợi ưu tiên get file trong folder trước
		uris.pSeparate = NULL;
		uris.f_Se = false;
		// end add folder
	}
}

void MyHTTP::Close()
{
	closesocket(this->m_SocketID);
}