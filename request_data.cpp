#include "request_data.hpp"

request_data::request_data(){}
request_data::request_data(int fd)
        : loop(0),started(false), finished(false),fd(fd), res_file_fd(-1) , res_started(false)
{}

request_data &request_data::operator=(const request_data& a)
{
    this->started = a.started;
    this->finished = a.finished;
    this->fd = a.fd;
    this->res_file_fd = a.res_file_fd;
    this->res_started = a.res_started;
    return *this;
}