#include "Directory.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <vector>
#include <stdio.h>
#include <algorithm> 
#include <unistd.h>

using namespace std;

Directory::Directory(string fp){
	this->filepath = fp;
	this->exists = true;
	this->html = formatHTML(filepath);
	this->fileSize = html.length();
	this->accessDate = getDate();
	this->metadata = string("Content-Type: ") + "text/html;"
			+ " charset=utf-8" + "\r\n"
			+ "Content-Language: en-US" + "\r\n"
			+ "Content-Length: " + fileSize + "\r\n"
			+ "Date: " + accessDate + "\r\n\r\n";	
}

bool sortVector(string a, string b){
	return tolower(a[0]) < tolower(b[0]);
}

string Directory::formatHTML(string fp){	
	DIR *dirstream = opendir(fp.c_str());
	struct dirent *tarDir = readdir(dirstream);

	vector<string> subDirs;
	vector<string> files;

	if(fp.back() != '/') fp += '/';

	while(tarDir) {
		 if (tarDir->d_name[0] == '.' || tarDir->d_name[0] == '#' || tarDir->d_name[strlen(tarDir->d_name)-1] == '~'){
		 	tarDir = readdir(dirstream);
		 	continue;
		 }

		if(isDirectory(fp + string(tarDir->d_name))){
			subDirs.push_back(string(tarDir->d_name));
		} else {
			files.push_back(string(tarDir->d_name));
		}
		tarDir = readdir(dirstream);
	}
	closedir(dirstream);

	sort(subDirs.begin(), subDirs.end(), sortVector);
	sort(files.begin(), files.end(), sortVector);

	string html = string("<a href=" + fp + "../>Parent Directory<br/><br/></a>\n")
			+ "<ul style=\"list-style: none; padding-left:0;\">\n";
	for(auto i:subDirs){
		html += "<li><a href=" + fp + i + ">"+ i + "</a></li>\n";
	}
	html += "<br/>\n";
	for(auto i:files){
		html += "<li><a href=" + fp + i + ">"+ i + "</a></li>\n";
	}
	html += "</u>";
	return html;
}

void Directory::sendDirItem(int fd){
	int error = write(fd, this->html.c_str(), this->html.length());
	if(error <= 0) {
		perror("Bad write");
		return;
	}
}

Directory::~Directory(){}
