#include <iostream>

#include "pipeinfo.hpp"
#include "named_pipe.hpp"

#define BUFSIZE 120

using namespace boost::interprocess;

int main() {
  named_pipe pipe(pipename);

  if (pipe.get_name() != pipename) {
    std::cerr << "error: received " << pipe.get_name().c_str() << "; expected " << pipename << std::endl;
    return 1;
  }

  char * buff = new char[BUFSIZE];

  pipe.write("CREATE MATRIX MyMat", 19);
  int len=pipe.read(buff,BUFSIZE);
  std::string res(buff,len);
  if(res=="CREATE SUCCESS") {
    std::cout << "MyMat Matrix created on server. Calling \"print\" action" << std::endl;
    pipe.write("CALL MyMat print", 16);  
  }

  // examples for how to use async calls:

  // auto wres=pipe.async_write(clientString, strlen(clientString));
  // wres.get();

  // auto res=pipe.async_read(buff,BUFSIZE);
  // auto tup=res.get();

  // printf("Received: %s\n", std::get<char*>(tup));
  // printf("Received: %s\n", buff);

  delete[] buff;

  return 0;
}
