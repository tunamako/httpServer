#pragma once
#include <string>
#include <map>
#include <vector>
using namespace std;

class DirItem {
public:
	const int BUFSIZE = 10240;
	string filepath;
	string fileSize;
	string filetype;
	string accessDate;
	string metadata;
	bool exists;
	string translatePath(string fp);
	vector<string> splitStrBy(char  *aString, const char *delimiters);
	string getDate();
	virtual void sendDirItem(int fd);
	bool isDirectory(string fp);
	DirItem();
	~DirItem(void);
};
