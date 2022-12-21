#include "request_data.hpp"

request_data::request_data(){}
request_data::request_data(int fd)
        : finished(false),fd(fd), res_file_fd(-1) , res_started(false)
{

    // std::cout << "filename:" << this->req_file << std::endl;
    // this->req_file.open(this->req_file.c_str(),  O_CREAT | O_WRONLY);

    // if (this->req_file < 0){

    // 	std::cout << "OPEN: " << std::strerror(errno) << '\n';
    // 	exit(1);
    // }
}

request_data &request_data::operator=(const request_data& a)
{
    // (void)a;
    std::cout << "OPERATOR\n";
    this->fd = a.fd;
    this->finished = a.finished;
    // this->req_file = a.req_file;
    this->res_file_fd = a.res_file_fd;
    return *this;
}