CXX = g++
CXXFLAGS = -std=c++11 -Wall -g 

server: server.o  Request.o DirItem.o File.o  Directory.o
	$(CXX) $(CXXFLAGS) -o server server.o  Request.o DirItem.o File.o Directory.o

server.o: server.cc  Request.h DirItem.h File.h Directory.h
	$(CXX) $(CXXFLAGS) -c server.cc

Request.o: Request.h

DirItem.o: DirItem.h

File.o: File.h

Directory.o: Directory.h

clean:
	rm *.o server