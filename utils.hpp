#ifndef UTILS_DEF
#define UTILS_DEF

#include <string>
#include <vector>
#include <sstream>
#include <istream>
#include <fstream>
#include <random>
#include <chrono>

namespace utils {
	
    typedef std::chrono::time_point<std::chrono::system_clock> timestamp;
    
    void trim_right(std::string& str);
	void trim_left(std::string& str);
	std::string trim(std::string& str);
	std::vector<std::string> split(const std::string &s, char delim);
    std::vector<std::string> split(const std::string &s, std::string& delim);
	std::vector<std::vector<std::string> > read_config(const std::string &path);
	bool starts_with(std::string &s1, const std::string &s2);
	bool ends_with(std::string &s1, const std::string &s2);
	void replace_all(std::string& s1, const std::string &s2, const std::string &s3);
	void remove_quotes(std::string &s);
	
	template<typename VectorType>
	VectorType select_random(const std::vector<VectorType>& _vec);

	int random(int);
    size_t random(size_t);

	bool iequals(const std::string& a, const std::string& b);

    long long seconds_since(const timestamp& _t);
    long long minutes_since(const timestamp& _t);
    long long mili_seconds_since(const timestamp& _t);

    template<typename T>
    T* pointerFromString(const std::string& _str);
    
    template<typename T>
    std::string pointerToString(T* _ptr);
};
#endif