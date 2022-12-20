#include "utils.hpp"
#include <fstream>
#include <cstdio>
#include <arpa/inet.h>
#include <exception>
#include <iterator>
#include <string>
#include <strings.h>
#include <sys/fcntl.h>
#include <sys/select.h>
// #include <sys/_endian.h>
#include <cerrno>
#include <sys/_select.h>
#include <algorithm>
// #include <sys/_types/_fd_def.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>
#include <fcntl.h>
#include <iostream>
// #include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write(), close()
#include <utility>
#include <vector>
#include <map>
class request_data {
public:
	std::ofstream req_file;
	// std::string req_file;
	// std::string req_file_name;
	// int req_file_fd;
	bool finished;
	int fd;
	int res_file_fd;


public:
	request_data(){}
	request_data(int fd)
			:  req_file("a"),finished(false),fd(fd), res_file_fd(-1) 
	{
		// this->req_file.open("/tmp/" + RandomString(), std::ios::out | std::ios::in);
		// std::cout << "filename:" << this->req_file << std::endl;
		// this->req_file = open(this->req_file.c_str(),  O_CREAT | O_WRONLY);

		// if (this->req_file < 0){

       	// 	std::cout << "OPEN: " << std::strerror(errno) << '\n';
		// 	exit(1);
		// }
	}
};
int main () {
   char data[100];

    request_data a(5);
   // open a file in write mode.
   std::ofstream outfile;
   outfile.open("afile.dat");

   std::cout << "Writing to the file" << std::endl;
   std::cout << "Enter your name: "; 
   std::cin.getline(data, 100);

   // write inputted data into the file.
   outfile << data << std::endl;

   std::cout << "Enter your age: "; 
   std::cin >> data;
   std::cin.ignore();
   
   // again write inputted data into the file.
   outfile << data << std::endl;

   // close the opened file.
   outfile.close();

   // open a file in read mode.
   std::ifstream infile; 
   infile.open("afile.dat"); 
 
   std::cout << "Reading from the file" << std::endl; 
   infile >> data; 

   // write the data at the screen.
   std::cout << data << std::endl;
   
   // again read the data from the file and display it.
   infile >> data; 
   std::cout << data << std::endl; 

   // close the opened file.
   infile.close();

   return 0;
}