#include "request_data.hpp"

request_data::request_data(){}
request_data::request_data(int fd)
        : started(false), finished(false),res_started(false), fd(fd), res_file_fd(-1), loop(0)
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