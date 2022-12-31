#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <strings.h>
#include <unistd.h>
#include <fstream>
#define BUFF_SIZE 1024

std::string getCgiResponse(const std::string& filename);
void        setMetaVariables();