// #pragma once
#include "utils.hpp"
std::string random_string()
{
    char alpha[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
                          'o', 'p', 'q', 'r', 's', 't', 'u',
                          'v', 'w', 'x', 'y', 'z' };
    std::string result = "";
    for (int i = 0; i<10; i++)
        result = result + alpha[rand() % 26];
    return result;
}
void    clear_file(std::string &filename)
{
    // (void)req;
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::trunc);
    ofs << "";
    // ofs.close();
}