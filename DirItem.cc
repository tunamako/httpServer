#include "DirItem.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <string.h>
#include <regex>
#include <fstream>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>

using namespace std;
DirItem::DirItem() {}

string DirItem::translatePath(string fp){
	if(fp[1] != '~')
		return fp;

	char *nonConst = new char[fp.size() + 1];
	strncpy(nonConst, fp.c_str(), fp.size() + 1);
	vector<string> pathComponents = splitStrBy(nonConst, " /\n.");
	delete[] nonConst;

	struct passwd *entry = getpwent();
	regex expr("(\\/~\\w+)");
	const char *reqName = (pathComponents[0].erase(0, 1)).c_str();

	while(entry){
		if(strcmp(entry->pw_name, reqName) == 0){
			endpwent();
			return regex_replace(fp, expr, entry->pw_dir);
		}
		entry = getpwent();
	}
	endpwent();
	return fp;
}

vector<string> DirItem::splitStrBy(char  *aString, const char *delimiters) {
	char *temp = " ";
	vector<string> returnVector;

	returnVector.push_back(string(strtok(aString, delimiters)));

	while(1) {
		temp = strtok(NULL, delimiters);
		if(!temp) break;	
		returnVector.push_back(string(temp));
	}
	return returnVector;
}

string DirItem::getDate(){
	time_t now = time(0);
	char *rawDate = ctime(&now);
	vector<string> dateComponents = splitStrBy(rawDate, " \n");

	return dateComponents[0] + ", "	//Name of day
		+ dateComponents[2] + " "	//Day number
		+ dateComponents[1] + " "	//Month
		+ dateComponents[4] + " "	//Year
		+ dateComponents[3]		//Time
		+ " -0500";
}

void DirItem::sendDirItem(int fd){}

bool DirItem::isDirectory(string fp) {
	struct stat st;
	stat(fp.c_str(), &st);
	return S_ISDIR(st.st_mode);
}

DirItem::~DirItem(){}