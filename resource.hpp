
#ifndef RESOURCE_HEADER
#define RESOURCE_HEADER


#include <map>
#include <vector>

#include <boost/variant.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using var_type = boost::variant<int, double, std::string>;

class Resource {
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & attributes;
	}
public:
    std::string name;
    std::map<std::string, var_type> attributes;
    std::map<std::string, std::function<void()>> actions;
    void print_name() {
        std::cout << name << std::endl;
    }
	Resource() {

	}
    virtual ~Resource(){}
};

class MatrixResource: public Resource {
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Resource>(*this);
		ar & cells;
	}
public:
    std::vector<double> cells;
    MatrixResource() {
        cells.resize(25,0);
        boost::get<int>(attributes["rows"])=5;
        boost::get<int>(attributes["cols"])=5;
        actions["print"]=[this]{print();};
		actions["make_identity"] = [this] {make_identity(); };
    }
	void make_identity() {
		for (size_t i = 0; i < 25; i++)
		{
			cells[i] = (i % 6 == 0) ? 1 : 0;
		}
	}
    void print() {
        int rows = boost::get<int>(attributes["rows"]);
        int cols = boost::get<int>(attributes["cols"]);
        for(int i=0;i<rows;i++) {
            for(int j=0;j<cols;j++) {
                std::cout << cells[i*rows+j] << " ";
            }
            std::cout << std::endl;
        }
    }
    ~MatrixResource(){}
};


#endif