#pragma once
#include "DirItem.h"
#include <string>
#include <map>
using namespace std;

class Request {
public:
	DirItem *target;
	string statusMsg;
	string version;
	map<string, string> parts;
	map<string, string> parseRequest(char *buffer);
	void logRequest();
	void sendResponse(int fileDescriptor);
	string getStatusMsg();
	Request(char *buffer);
	~Request(void);
};
