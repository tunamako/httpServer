#include "Request.h"
#include "Directory.h"
#include "File.h"
#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>


using namespace std;
Request::Request(char *aBuffer, ofstream *aLogFile) {
	this->buffer = aBuffer;
	parseRequest();
	this->version = parts["Version"];
	if (this->target->isDirectory(parts["Filepath"]))
		this->target = new Directory(parts["Filepath"]);
	else 
		this->target = new File(parts["Filepath"]);

	this->statusMsg = getStatusMsg();
	logRequest(aLogFile);
}

string Request::getStatusMsg(){
	return (target->exists)
		? (parts.count("Command") == 0)
			? "400 Bad Request\r\n\r\nBad Request"
			: "200 OK\r\n"
		: "404 File Not Found\r\n\r\nFile not found";
}

void Request::parseRequest(){
	string buf = string(buffer);
	istringstream sstream(buf);
	string line;
	smatch match;
	regex expr("(?:(.*):\\s(.*\\b))|(GET)\\s(\\S+)\\s(\\S+)\\b");
	map<string, string> parts;

	while (getline(sstream, line)) {
		if(regex_search(line, match, expr)){
			if(match[3] == "GET") {
				parts.emplace("Command", match[3]);
				parts.emplace("Filepath", this->target->translatePath(string(match[4])));
				parts.emplace("Version", match[5]);
			} else {
				parts.emplace(match[1], match[2]);	
			}
		}
	}
	this->parts = parts;
}

void Request::logRequest(ofstream *aLogFile){
	cout << "\n" + parts["Command"] + " "
		+ parts["Filepath"] + " "
		+ parts["Version"] + "\n"
		+ "User-Agent: " + parts["User-Agent"] + "\n";

	string referer = (parts.count("Referer") != 0)
		? parts["Referer"] 
		: "";

	string entry =  string("127.0.0.1")
		+ " - - [" + target->accessDate + "] \"" 
		+ parts["Command"] + " "
		+ parts["Filepath"] + " "
		+ parts["Version"] + "\" " 
		+ statusMsg[0] + statusMsg[1] + statusMsg[2]
		+ " " + target->fileSize + " "
		+ " \"" + referer + "\" "
		+ "\"" + parts["User-Agent"] + "\"\n";
		
	(*aLogFile) << entry;
	aLogFile->flush();
}

void Request::sendResponse(int fileDescriptor){
	string msg = string("HTTP/1.0 ") + statusMsg;
	if(statusMsg[0] != '2' && version == "HTTP/0.9"){
		return;
	} else if (statusMsg[0] != '2') {
		int writeLen = write(fileDescriptor, msg.c_str(), msg.length());
		if(writeLen == -1){
			perror("Bad write");
		}
		return;
	}

	if(version != "HTTP/0.9"){
		int writeLen = write(fileDescriptor, msg.c_str(), msg.length());
		if(writeLen == -1){
			perror("Bad write");
			return;
		}
		writeLen = write(fileDescriptor, target->metadata.c_str(), target->metadata.length());
		if(writeLen == -1){
			perror("Bad write");
			return;
		}

	}
	target->sendDirItem(fileDescriptor);
}

Request::~Request(){}
