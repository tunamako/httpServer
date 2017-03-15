#include "Request.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <csignal>
#include <fstream>

using namespace std;
const int BUFSIZE = 1024;

int makeSocket(const char *port){
	const int BACKLOG = 3;	
	int s; 		
	struct sockaddr_in sa; 
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;

	hp = gethostbyname("127.0.0.1");

	sscanf(port, "%d", &portnum);
	if (portnum ==  0) {
		sp=getservbyname(port, "tcp");
		portnum = ntohs(sp->s_port);
	}
	sa.sin_port = htons(portnum);

	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	s = socket(hp->h_addrtype, SOCK_STREAM, 0);

	ret = bind(s, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0){
		perror("Could not connect");
		exit(1);
	}
	listen(s, BACKLOG);
	cout << "Waiting for connection on port " << port << endl;
	return s;
}
int makeFileDescriptor(int sock){
	int sockAddrLen;
	struct sockaddr_in sockAddr;
	sockAddrLen = sizeof(sockAddr);
	return accept(sock, (struct sockaddr *)&sockAddr, (unsigned int *)&sockAddrLen);
}

int main(int argc, char *argv[]) {
	char buffer[BUFSIZE];
	int sock = makeSocket((string("1282") + argv[1]).c_str());
	ofstream *aLogFile = new ofstream("log.txt", ios_base::app);

	//We dont care about child processes, have the kernel kill the zambies
	//From http://stackoverflow.com/questions/6718272/c-exec-fork-defunct-processes
	signal(SIGCHLD, SIG_IGN);

	while(1){
		int fileDescriptor = makeFileDescriptor(sock);

		if(fork() == 0){
			//Child
			int readLen = read(fileDescriptor, buffer, BUFSIZE);
	
			if(readLen == -1){
				perror("Bad read");
				close(fileDescriptor);
				continue;
			}
			if(readLen == 0){
				cout << "Empty Request" << endl;
				close(fileDescriptor);
				continue;
			}
			buffer[readLen] = 0;
			Request *aRequest = new Request(buffer, aLogFile);
			aRequest->sendResponse(fileDescriptor);

			close(fileDescriptor);
			exit(0);
		} else {
			//Parent
			close(fileDescriptor);
		}
	}
	return 0;
}