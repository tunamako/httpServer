#include "File.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <string.h>
#include <regex>
#include <fstream>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
File::File(string fp) {
	struct stat st;

	this->filepath = fp;
	this->exists = stat(filepath.c_str(), &st) == 0;

	char *nonConst = new char[fp.size() + 1];
	strncpy(nonConst, fp.c_str(), fp.size() + 1);
	this->filetype = splitStrBy(nonConst, " /\n.").back();
	delete[] nonConst;

	this->fileSize = exists
		? to_string(st.st_size)
		: "";
	this->accessDate = getDate();
	this->metadata = getMetaData();	
}

string File::getContentType(){
	ifstream file;
	string line;
	regex expr("(\\S*).*\\s"+filetype+".*");

	file.open("/etc/mime.types");
	assert(file.good());
	smatch match;

	while (getline(file, line)) {
		if(regex_search(line, match, expr))
			return match[1];
	}
	return "";
}

string File::getMetaData(){
	string contentType = getContentType();
	return (contentType.c_str())
		? string("Content-Type: ") + contentType + ";"
			+ " charset=utf-8" + "\r\n"
			+ "Content-Language: en-US" + "\r\n"
			+ "Content-Length: " + fileSize + "\r\n"
			+ "Date: " + accessDate + "\r\n\r\n"
		: "";
}

void File::sendDirItem(int fd){
	char buffer[BUFSIZE];
	int file = open(filepath.c_str(), O_RDONLY);
	if (file == -1){
		perror("Could not open file");
		return;
	}

	int fileLen = read(file, buffer, BUFSIZE);

	while (fileLen > 0) {
		int error = write(fd, buffer, fileLen);
		if(error <= 0 || fileLen != error) {
			perror("Bad write");
			return;
		}
		fileLen = read(file, buffer, BUFSIZE);
	}
}

File::~File(){}