
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/system/windows_error.hpp>

#include "pipeinfo.hpp"
#include "named_pipe.hpp"

#include "resource.hpp"

#define BUFSIZE 120

using namespace boost::interprocess;
std::map<std::string,Resource*> active_objects;

#include "server.hpp"


int main() {
  named_pipe_server server(pipename);

  // ACTION:
  //  LIST CLASS
  //  LIST ACTION <OBJECT>
  //  CREATE <TYPE> <NAME>
  //  GET <NAME> <ATTRIBUTE>
  //  *GET <NAME>
  //  *TYPE <NAME> <ATTRIBUTE>
  //  *TYPE <NAME>
  //  CALL <NAME> <ACTION>
  //  UPDATE <NAME> <ATTRIBUTE> <NEW DATA>

  std::cout<<"Server created..."<< std::endl;

  named_pipe pipe = server.accept();
   std::cout << "Client connected." << std::endl;

  if (pipe.get_name() != pipename) {
    std::cerr << "error: received " << pipe.get_name().c_str() << "; expected " << pipename << std::endl;
    return 2;
  }

  while(true){

    char *buff = new char[BUFSIZE];

    size_t len=pipe.read(buff, BUFSIZE);
    if(len==0) break;
    std::cout << "Read " << len << " bytes." << std::endl;
    std::istringstream iss(std::string(buff,len));

    std::string request_type;
    iss >> request_type;
    std::cout << "Request type: " << request_type << std::endl;

    if(request_type=="LIST") {
      std::string type;
      iss >> type;
      if(type=="CLASS") {
        list_class(pipe);
      }
      else if(type=="ACTION") {
        std::string class_name;
        iss >> class_name;
        list_actions(pipe,class_name);
      }
    }
    if(request_type=="GET") {
      std::string object_name;
      iss >> object_name;
      if(!iss.eof()) {
        std::string attr_name;
        iss >> attr_name;
        get_object_attribute(pipe,object_name,attr_name);
      }
      // TODO: GET WHOLE OBJ
    }
    else if(request_type=="TYPE") {
      // TODO: Not Implemented
      std::string object_name;
      iss >> object_name;
      if(!iss.eof()) {
        std::string attr_name;
        iss >> attr_name;
        // TYPE OBJ ATTR
      }
      // TYPE OBJ
    }
    else if(request_type=="CREATE") {
      std::string type,object_name;
      iss >> type >> object_name;
      create(pipe,type,object_name);
    }
    else if(request_type=="CALL") {
      std::string object_name,action;
      iss >> object_name >> action;
      call_object_action(pipe,object_name,action);
    }
    else if(request_type=="UPDATE") {
      std::string object_name,attr;
      iss >> object_name >> attr;

      std::string new_data(iss.str().substr(iss.tellg()));

      update_object_attribute(pipe,object_name,attr,new_data);
    }

    delete[] buff;
  }

  return 0;
}
