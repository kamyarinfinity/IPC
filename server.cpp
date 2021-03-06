
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/system/windows_error.hpp>

#include "pipeinfo.hpp"
#include "named_pipe.hpp"

#include "resource.hpp"

BOOST_CLASS_EXPORT(MatrixResource)

#define BUFSIZE 512

using namespace boost::interprocess;
std::map<std::string,Resource*> active_objects;

#include "server.hpp"


int main() {
  named_pipe_server server(pipename);

  // ACTION:
  //  LIST CLASS
  //  LIST OBJECT
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
	size_t len = 0;
	try {
		len = pipe.read(buff, BUFSIZE);
	}
	catch (...) {
		break;
	}
    if(len==0) break;

    std::cout << "Read " << len << " bytes." << std::endl;
    std::istringstream iss(std::string(buff,len));

    std::string request_type;
    iss >> request_type;
	std::cerr << "Full Request: " << iss.str() << std::endl;
    std::cout << "Request type: " << request_type << std::endl;

    if(request_type=="LIST") {
      std::string type;
      iss >> type;

	  std::cout << "List type: " << type << std::endl;

      if(type=="CLASS") {
        list_class(pipe);
      }
	  else if (type == "OBJECT") {
		list_object(pipe);
	  }
      else if(type=="ACTION") {
        std::string class_name;
        iss >> class_name;
        list_action(pipe,class_name);
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
	  else {
		  get_object(pipe, object_name);
	  }
    }
    else if(request_type=="TYPE") {
      std::string object_name;
      iss >> object_name;
      if(!iss.eof()) {
        std::string attr_name;
        iss >> attr_name;
		get_type_object_attribute(pipe, object_name, attr_name);
      }
	  else {
		  get_type_object(pipe, object_name);
	  }
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

  std::cout << "Connection closed. Exiting." << std::endl;

  return 0;
}
