#pragma once
#include <iostream>
#include <fstream>

class request_data
{
    public:

        bool started;
        bool finished;
        bool res_started;
        int fd; // sock_fd
        // std::ofstream req_file;
        // ============== mmap header
        // ============== ofstream fl;
        int req_file_fd;
        int res_file_fd;
        std::string req_file_name;
        unsigned long long loop;
        
    public:
        request_data();
        request_data(int fd);
        request_data &operator=(const request_data& a);
};