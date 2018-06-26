
#ifndef RESOURCE_HEADER
#define RESOURCE_HEADER


#include <map>
#include <vector>

#include <boost/variant.hpp>

using var_type = boost::variant<int, double, std::string>;

class Resource {
public:
    std::string name;
    std::map<std::string, var_type> attributes;
    std::map<std::string, std::function<void()>> actions;
    void print_name() {
        std::cout << name << std::endl;
    }
    virtual ~Resource(){}
};

class MatrixResource: public Resource {
public:
    std::vector<double> cells;
    MatrixResource() {
        cells.resize(25,0);
        boost::get<int>(attributes["rows"])=5;
        boost::get<int>(attributes["cols"])=5;
        actions["print"]=[this]{print();};
    }
    void print() {
        int rows = boost::get<int>(attributes["rows"]);
        int cols = boost::get<int>(attributes["cols"]);
        for(int i=0;i<rows;i++) {
            for(int j=0;j<cols;j++) {
                std::cout << cells[i*rows+cols] << " ";
            }
            std::cout << std::endl;
        }
    }
    ~MatrixResource(){}
};

#endif