#pragma once
#include <iostream>
#include <fstream>

class request_data
{
    public:

        // std::ofstream req_file;
        unsigned long long loop;
        int req_file_fd;
        bool started;
        bool finished;
        int fd;
        int res_file_fd;
        std::string req_file_name;
        bool res_started;
    public:
        request_data();
        request_data(int fd);
        request_data &operator=(const request_data& a);
};