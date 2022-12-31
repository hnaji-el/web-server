#include "cgi.hpp"
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>


extern char **environ;
void setMetaVariables()
{
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("SERVER_SOFTWARE", "Webserv", 1);
    setenv("PATH_TRANSLATED", "index.php", 1);
    setenv("CONTENT_LENGTH", "23", 1);
    // setenv("CONTENT_TYPE", "", 1);
    setenv("REDIRECT_STATUS", "200", 1);
    setenv("REQUEST_METHOD", "GET", 1);
    //  setenv("QUERY_STRING", "", 1);
    // setenv("SCRIPT_FILENAME", "index.php" , 1);
    // setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    // setenv("PATH_INFO", "test.php", 1);
    // setenv("SCRIPT_FILENAME", scriptName.c_str(), 1);
    // setenv("PATH_INFO", sriptname.c_str() , 1);
}

std::string getCgiResponse(const std::string& uriFile,const std::string& bodyFile)
{
    setMetaVariables();
    int pipeFd[2];
    char buff[BUFF_SIZE + 1];
    int fdBody;
    int status;
    char *args[2] = {(char*)"./php-cgi", NULL};
    int cgiFileFd = open(bodyFile.c_str(), O_RDONLY);
    bzero(&buff, BUFF_SIZE);
    pipe(pipeFd);

    std::string cgiFileName = "cgi.txt"; // random naming
    int pid = fork();
    if (pid == 0)
    {
        fdBody = open(cgiFileName.c_str(), O_WRONLY  | O_CREAT| O_TRUNC, 0644);

        dup2(fdBody, 1);
        dup2(cgiFileFd, 0);
        close(pipeFd[1]);
        execve(args[0], args, environ);
        std::cerr << "ERROE EXCVE" << std::endl;
        exit(1);
    }
    else
    {
        close(pipeFd[1]);
        // waitpid(pid, &status, WNOHANG);
        wait(&status);
        if (WEXITSTATUS(status) == 1) // WNOHANG
        {
            std::cout << "HELLO\n";
            return "";
        }
        close(cgiFileFd);
    }
    return cgiFileName;
}

int main()
{
    std::string CgiReponseFile = getCgiResponse("index.php","khawi");
    if (CgiReponseFile.empty())
        std::cout << "EMPTY STRING\n";
    // std::ifstream f(CgiReponseFile);
    // if (f.is_open())
    //     std::cout << f.rdbuf();
}