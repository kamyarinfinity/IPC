#include <iostream>

#include "pipeinfo.hpp"
#include "named_pipe.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <sstream>

#include "resource.hpp"

BOOST_CLASS_EXPORT(MatrixResource)

#define BUFSIZE 512

using namespace boost::interprocess;

std::string read_str(named_pipe &pipe) {
	char * buff = new char[BUFSIZE];
	int len = pipe.read(buff, BUFSIZE);
	std::string res(buff, len);
	delete[] buff;
	return res;
}

void test_create(named_pipe &pipe) {
	pipe.write("CREATE MATRIX MyMat", 19);
	assert(read_str(pipe) == "CREATE SUCCESS");
}

void test_list_class(named_pipe &pipe) {
	pipe.write("LIST CLASS", 10);
	assert(read_str(pipe) == "DICTIONARY MATRIX");
}

void test_list_object(named_pipe &pipe) {
	pipe.write("LIST OBJECT", 11);
	assert(read_str(pipe) == "MyMat");
}

void test_list_action(named_pipe &pipe) {
	pipe.write("LIST ACTION MyMat", 17);
	std::string response = read_str(pipe);
	assert(response == "print make_identity" || response == "make_identity print");
}

void test_type(named_pipe &pipe) {
	pipe.write("TYPE MyMat", 10);
	assert(read_str(pipe) == "MATRIX");

	pipe.write("TYPE MyMat rows", 15);
	assert(read_str(pipe) == "INT");
}

// requires test_call to work
void test_get(named_pipe &pipe) {
	pipe.write("CALL MyMat make_identity", 24);
	assert(read_str(pipe) == "CALL SUCCESS");

	pipe.write("GET MyMat", 9);
	std::stringstream ss(read_str(pipe));

	boost::archive::binary_iarchive ar(ss);
	ar.register_type<MatrixResource>();
	Resource *r = nullptr;
	ar >> r;

	// make sure the matrix has the updated values
	assert(r != nullptr && ((MatrixResource*)r)->cells[0] == 1);
}

void test_get_attr(named_pipe &pipe) {
	pipe.write("GET MyMat rows", 14);
	assert(std::stoi(read_str(pipe)) == 5);

	pipe.write("GET MyMat cols", 14);
	assert(std::stoi(read_str(pipe)) == 5);
}

void test_call(named_pipe &pipe) {
	pipe.write("CALL MyMat print", 16);
	assert(read_str(pipe) == "CALL SUCCESS");
}

void test_update(named_pipe &pipe) {
	pipe.write("UPDATE MyMat rows 7", 19);
	assert(read_str(pipe) == "UPDATE SUCCESS");

	pipe.write("GET MyMat rows", 14);
	assert(std::stoi(read_str(pipe)) == 7);
}

void test_async(named_pipe &pipe) {
	 auto wres=pipe.async_write("GET MyMat cols", 14);
	 wres.get();

	 char *buff = new char[BUFFSIZE];
	 auto res=pipe.async_read(buff,BUFSIZE);
	 auto response=res.get();
	 std::string resp_str(buff, std::get<int>(response));

	 assert(std::stoi(resp_str) == 5);
}

int main() {
  named_pipe pipe(pipename);

  if (pipe.get_name() != pipename) {
    std::cerr << "error: received " << pipe.get_name().c_str() << "; expected " << pipename << std::endl;
    return 1;
  }

  test_list_class(pipe);
  std::cout << "Test \"list_class\" successful." << std::endl;
  test_create(pipe);
  std::cout << "Test \"create\" successful." << std::endl;
  test_list_object(pipe);
  std::cout << "Test \"list_object\" successful." << std::endl;
  test_list_action(pipe);
  std::cout << "Test \"list_action\" successful." << std::endl;
  test_type(pipe);
  std::cout << "Test \"type\" successful." << std::endl;
  test_get_attr(pipe);
  std::cout << "Test \"get_attr\" successful." << std::endl;

  test_call(pipe);
  std::cout << "Test \"call\" successful." << std::endl;
  test_get(pipe);
  std::cout << "Test \"get\" successful." << std::endl;
  test_update(pipe);
  std::cout << "Test \"update\" successful." << std::endl;

  // async test
  test_async(pipe);
  std::cout << "Test \"async\" successful." << std::endl;

  return 0;
}
