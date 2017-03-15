#pragma once
#include "DirItem.h"
#include <string>
#include <map>
#include <fstream>
using namespace std;

class Request {
public:
	DirItem *target;
	char *buffer;
	string statusMsg;
	string version;
	map<string, string> parts;
	void parseRequest();
	void logRequest(ofstream *aLogFile);
	void sendResponse(int fileDescriptor);
	string getStatusMsg();
	Request(char *aBuffer, ofstream *aLogFile);
	~Request(void);
};
