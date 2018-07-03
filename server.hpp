#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/archive/binary_oarchive.hpp>


std::vector<std::string> SUPPORTED_CLASSES = {
  "DICTIONARY",
  "MATRIX"
};

void list_class(named_pipe &pipe) {
	std::string list_str = "";
	for (auto s : SUPPORTED_CLASSES) {
		list_str += s + " ";
	}
	if (list_str.length() > 0) list_str.pop_back();

	pipe.write(list_str.data(), list_str.length());
}

void list_object(named_pipe &pipe) {
	std::string list_str = "";
	for (auto e : active_objects) {
		list_str += e.first + " ";
	}
	if (list_str.length() > 0) list_str.pop_back();

	pipe.write(list_str.data(), list_str.length());
}

void list_action(named_pipe &pipe, std::string object_name) {
  std::string list_str="";
  auto pr=active_objects[object_name];
  for(auto entry:pr->actions) {
    list_str+= entry.first + " ";
  }
  if (list_str.length() > 0) list_str.pop_back();

  pipe.write(list_str.data(), list_str.length());
}
void create(named_pipe &pipe, std::string type, std::string object_name) {
  if(type=="DICTIONARY")
    active_objects[object_name]=new Resource();
  else if(type=="MATRIX")
    active_objects[object_name]=new MatrixResource();
  pipe.write("CREATE SUCCESS", 14);
}

void get_object(named_pipe &pipe, std::string object_name) {
	std::string result;
	{
		std::stringstream ss;
		boost::archive::binary_oarchive ar(ss);
		ar.register_type<MatrixResource>();
		ar << active_objects[object_name];
		result = ss.str();
		std::cout << "Writing serialized object with length " << result.length() << std::endl;
	}
	pipe.write(result.c_str(), result.length());
}

void get_object_attribute(named_pipe &pipe, std::string object_name, std::string attribute) {
  auto pr=active_objects[object_name];
  
  switch (pr->attributes[attribute].which()) {
    case 0: // int
    {
      int val=boost::get<int>(pr->attributes[attribute]);
      std::string val_str=std::to_string(val);
      pipe.write(val_str.c_str(), val_str.length());
      break;
    }
    case 1: // double
    {
      double val=boost::get<double>(pr->attributes[attribute]);
      std::string val_str=std::to_string(val);
      pipe.write(val_str.c_str(), val_str.length());
      break;
    }
    case 2: // string
    {
      std::string val=boost::get<std::string>(pr->attributes[attribute]);
      pipe.write(val.c_str(), val.length());
      break;
    }
  }
}

void call_object_action(named_pipe &pipe, std::string object_name, std::string action) {
  active_objects[object_name]->actions[action]();
  pipe.write("CALL SUCCESS", 12);
}

void update_object_attribute(named_pipe &pipe, std::string object_name, std::string attribute, std::string data) {
  auto pr=active_objects[object_name];
  
  switch (pr->attributes[attribute].which()) {
    case 0: // int
    {
      int new_val=std::stoi(data);
      boost::get<int>(pr->attributes[attribute])=new_val;
      break;
    }
    case 1: // double
    {
      double new_val=std::stod(data);
      boost::get<double>(pr->attributes[attribute])=new_val;
      break;
    }
    case 2: // string
    {
      boost::get<std::string>(pr->attributes[attribute])=data;
      break;
    }
  }
  pipe.write("UPDATE SUCCESS", 14);
}

void get_type_object_attribute(named_pipe &pipe, std::string object_name, std::string attribute) {
	auto pr = active_objects[object_name];

	switch (pr->attributes[attribute].which()) {
	case 0: // int
		pipe.write("INT",3);
		break;
	case 1: // double
		pipe.write("DOUBLE", 6);
		break;
	case 2: // string
		pipe.write("STRING", 6);
		break;
	default:
		pipe.write("[UNIDENTIFIED TYPE]", 20);
	}
}
void get_type_object(named_pipe &pipe, std::string object_name) {
	if (typeid(*active_objects[object_name]) == typeid(MatrixResource))
		pipe.write("MATRIX",6);
	else if (typeid(*active_objects[object_name]) == typeid(Resource))
		pipe.write("DICTIONARY", 10);
	else
		pipe.write("[UNIDENTIFIED TYPE]", 20);
}

#endif