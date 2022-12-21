#include <cstdint>
#include <fstream>
#include <unistd.h>
#include <iostream>
int main () {
   std::ofstream ofs;
   ofs.open("test.txt", std::ofstream::out);
   while(1)
   {
      ofs << "HELLO\n";
      ofs.flush();
   }
   // ofs.open("test.txt", std::ofstream::out | std::ofstream::trunc);
   // ofs.close();
   // ofs.open("test.txt", std::ofstream::out | std::ofstream::trunc);
   std::ifstream infile("test.txt");
   char b;
   while(infile.get(b)) // much better !!
   {
      std::cout << b << std::endl;
   }
   // ofs.close();
   return 0;
}