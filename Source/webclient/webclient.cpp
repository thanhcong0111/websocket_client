#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "MyHTTP.h"
#include <iostream>

using namespace std;

// Hàm này thực hiện việc kết nối tới server và tải file về máy tính
void get_FileAndFolder(QueueURI& uris, int port, int versionHTTP);

int main(int argc, char* argv[])
{
	int choice;
	cout << "*********************WELCOME TO AN NEW ERA DOWNLOADING*********************" << endl;
	do
	{
		cout << "\t- What do you want?." << endl;
		cout << "0. Send the request to download file." << endl;
		cout << "1. Exit." << endl;
		cout << "\t- Let us know: ";
		cin >> choice;
		if (choice == 0)
		{
			int choice;
			int num_link = 0;
			do
			{
				cout << "How many links would you like to download: "; cin >> num_link;
				if (num_link < 1 || num_link > 2)
					cout << "\nOh sorry, but our app just support at most 2 links. Try enter again!\n" << endl;
			} while (num_link < 1 || num_link > 2);

			char mssv[30];
			strcpy(mssv, "21120396_21120419_21120446_");
			int versionHTTP = 1;

			if (num_link == 1)
			{
				char link1[1024];
				cout << "Link: ";
				cin >> link1;

				QueueURI uris;			// Hàng đợi lưu trữ các link
				URI uri;                // CTDL URI lưu trữ link ban đầu chương trình nạp vào
				uri.setURL(link1, mssv);
				uri.separateURL();
				uris.enQueue(uri);
				while (!uris.isEmpty())
				{
					get_FileAndFolder(uris, 80, versionHTTP);
				}
			}
			else if (num_link == 2)
			{
				//Nhap cac link
				char link1[1024];
				cout << "- Link 1: "; cin >> link1;
				char link2[1024];
				cout << "\n- Link 2: "; cin >> link2;

				QueueURI uris;			// Hàng đợi lưu trữ các link
				URI uri;                // CTDL URI lưu trữ link ban đầu chương trình nạp vào
				uri.setURL(link1, mssv);
				uri.separateURL();
				uris.enQueue(uri);

				QueueURI urist;			// Hàng đợi lưu trữ các link
				URI urit;               // CTDL URI lưu trữ link ban đầu chương trình nạp vào
				urit.setURL(link2, mssv);
				urit.separateURL();
				urist.enQueue(urit);

				while (!uris.isEmpty() && !urist.isEmpty())
				{
					get_FileAndFolder(uris, 80, versionHTTP);
					get_FileAndFolder(urist, 80, versionHTTP);
				}
			}
		}
		else if (choice == 1)
		{
			cout << "\nThank for using our app. " << endl;
			cout << "Sincerely.\n" << endl;
			exit;
		}
		else 
			cout << "\nLet enter again! " << endl;
	} while (choice != 0 && choice != 1);
	
	

	// Chương trình dùng tham số dòng lệnh chạy chương trình argv[0]: mssv  argv[1]: Link  argv[2]: versionHTTP

	// Ý tưởng của chương trình là sẽ nạp các link download vào 1 hàng đợi, khi nào hàng đợi hết 
	// phần tử thì dừng chương trình, ngược lại thì mỗi khi đọc dữ liệu từ server nếu nhận được
	// là folder thì sẽ lấy hết các link sub item trong folder nạp vào hàng đợi, nếu là file thì thực hiện
	// download file bình thường, và lấy link này ra khỏi hàng đợi

	system("pause");
	return 0;
}


void get_FileAndFolder(QueueURI& uris, int port, int versionHTTP)
{
	// Lấy link ra khỏi hàng đợi uris
	URI uri;
	uris.deQueue(uri);


	MyHTTP http(uri, port);		// Khai báo đối tượng MyHTTP

	WSADATA a = { 0 };
	// Initialize winsock
	WSAStartup(MAKEWORD(2, 2), &a);		// Khởi tạo winsock


	if (http.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP))		// Khởi tạo MyHTTP
	{
		// Connect tới Server
		if (http.Connect())
		{
			char messageGet[1024];			// Lưu trữ chuỗi REQUEST từ máy client
			http.makeRequest(messageGet, versionHTTP);		// Thực hiện format chuỗi REQUEST theo version và URI ban đầu
			http.Send(messageGet);		// Gửi chuỗi REQUEST tới Server
			
			char header[65535];
			http.GetHeader(header, sizeof(header));	// Lấy header RESPONSE từ Server


			if (http.isValidResponse(header, versionHTTP))	// Kiểm tra gói header RESPONSE có được không
			{
				bool hasChunked = false;
				if (versionHTTP == 1)
				{
					hasChunked = http.hasChunkedEncoding(header);	// Kiểm tra gói có ChunkedEncoding không?
				}
				cout << "Header ok!" << endl;
				http.writeData(uris, versionHTTP, hasChunked);		// Thực hiện việc lưu trữ file
			}
			else
			{
				cout << "Link Is Not OK" << endl;
			}
		}
		else
		{
			cout << "connect failed" << endl;
		}
		http.Close();
	}
	else
	{
		cout << "Init failed";
	}

	WSACleanup();
}

