#pragma once
#include "DirItem.h"
#include <string>
using namespace std;

class File: public DirItem{
public:
	string getContentType();
	string getFileSize();
	string getMetaData();
	virtual void sendDirItem(int fd);
	File(string fp);
	~File(void);
};
