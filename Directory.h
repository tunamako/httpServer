#pragma once
#include "DirItem.h"
#include <string>
using namespace std;

class Directory: public DirItem {
public:
	string html;
	bool pathIsDirectory(string filepath);
	string formatHTML(string fp);
	virtual void sendDirItem(int fd);
	Directory(string fp);
	~Directory(void);
};
