#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "URI.h"


// Constructor, Destructor
URI::URI()
{
	strcpy_s(this->url, "");
	strcpy_s(this->hostname, "");
	strcpy_s(this->pathname, "");
	strcpy_s(this->itemname, "");
	this->isFile = true;
}

URI::~URI()
{
}

void URI::setURL(char url[1024], char pre_ItemName[256])
{
	strcpy_s(this->url, url);
	strcpy_s(this->pre_ItemName, pre_ItemName);
}

bool URI::separateURL()
{
	isFile = true;		// Giả sử ban đầu là link tải file

	if (strlen(this->url) == 0)
		return false;

	bool flag = true;
	int strLen = strlen(this->url);

	// Lấy vị trí ngăn cách giữa host và path
	int index = 0;

	for (int i = 0; i < strLen; i++)
	{
		if (this->url[i] == '/')
		{
			if (this->url[i - 1] != '/' && this->url[i + 1] != '/')
			{
				index = i;
				break;
			}
		}
	}

	if (index == 0)
	{
		bool fHost = false;
		int iHostName = 0;
		for (int i = 0; i < strLen; i++)
		{
			if (this->url[i] == ':')
			{
				i += 2;
				fHost = true;
				continue;
			}
			if (fHost)
				this->hostname[iHostName++] = this->url[i];
		}
		this->hostname[iHostName] = 0;
		strcat_s(this->pathname, "/");
	}
	else
	{

		// Lấy hostname
		bool f_Host = false;
		int i_HostName = 0;
		for (int i = 0; i < index; i++)
		{
			if (this->url[i] == ':')
			{
				i += 2;
				f_Host = true;
				continue;
			}
			if (f_Host)
				this->hostname[i_HostName++] = this->url[i];
		}
		this->hostname[i_HostName] = 0;

		// Lấy pathname
		int j_Path = 0;
		for (int i = index; i < strLen; i++, j_Path++)
		{
			this->pathname[j_Path] = this->url[i];
		}
		this->pathname[j_Path] = 0;


		// Lấy itemname
		int i_Item = strLen - 1;

		if (this->url[i_Item] == '/' && this->pathname == "" || this->url[i_Item] == '/' && index > 1)
		{
			i_Item--;
			isFile = false;
		}

		int i_Index = i_Item;
		while (this->url[i_Index--] != '/');
		this->itemname[i_Item - i_Index - 1] = 0;
		i_Index++;
		while (i_Item > i_Index)
		{
			this->itemname[i_Item - i_Index - 1] = this->url[i_Item];
			i_Item--;
		}

		this->makeBeautyItemName();

	}
	if (_strcmpi(this->itemname, "") == 0)
		strcat_s(this->itemname, "index.html");
	
	
}

void URI::makeBeautyItemName()
{
	// Loại bỏ %20 = ' '
	int strLenLink = strlen(this->itemname);
	for (int i1 = 0; i1 < strLenLink; i1++)
	{
		if (this->itemname[i1] == '%')
		{
			this->itemname[i1] = ' ';
			for (int i2 = i1 + 3; i2 < strLenLink; i2++)
			{
				this->itemname[i2 - 2] = this->itemname[i2];
			}
			this->itemname[strLenLink - 2] = 0;
			strLenLink = strlen(this->itemname);
		}
	}
}
