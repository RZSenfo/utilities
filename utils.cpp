#include "utils.hpp"

namespace utils {
	void trim_right(std::string &str) {
		size_t endpos = str.find_last_not_of(" \t");
		if (std::string::npos != endpos) {
			str = str.substr(0, endpos + 1);
		}
	}

	void trim_left(std::string &str) {
		size_t startpos = str.find_first_not_of(" \t");
		if (std::string::npos != startpos) {
			str = str.substr(startpos);
		}
	}

	std::string trim(std::string &str) {
		trim_left(str);
		trim_right(str);
		return str;
	}

	std::vector <std::string> split(const std::string &s, char delim) {
		std::vector <std::string> elems;
		std::string next;

		for (std::string::const_iterator it = s.begin(); it != s.end(); it++) {
			// If we've hit the terminal character
			if (*it == delim) {
				// If we have some characters accumulated
				if (!next.empty()) {
					// Add them to the result vector
					elems.push_back(next);
					next.clear();
				}
			} else {
				// Accumulate the next character into the sequence
				next += *it;
			}
		}
		if (!next.empty())
			elems.push_back(next);
		return elems;

	}

	std::vector <std::string> split(const std::string &s, std::string &delim) {
		std::vector <std::string> elems;
		std::string next;

		if (delim.size() == 0) return elems;

		size_t _cur_pos = 0;
		for (std::string::const_iterator it = s.begin(); it != s.end(); it++) {
			// If we've hit the terminal character
			if (*it == delim.at(0) && _cur_pos < (s.size() - delim.size())) {

				std::string _cur_delim = s.substr(_cur_pos, delim.size());
				if (_cur_delim == delim) {
					// If we have some characters accumulated
					if (!next.empty()) {
						// Add them to the result vector
						elems.push_back(next);
						next.clear();
					}
				} else {
					next += *it;
				}
			} else {
				// Accumulate the next character into the sequence
				next += *it;
			}
			_cur_pos++;
		}
		if (!next.empty())
			elems.push_back(next);
		return elems;

	}

	std::vector <std::vector<std::string>> read_config(const std::string &path) {

		std::vector <std::vector<std::string>> entries;


		//TODO check path for security issues
		//must be relative to the config folder
		std::ifstream moduleConfig(path.c_str());
		if (moduleConfig.is_open()) {
			std::string line;
			std::string key = "";
			std::string value = "";
			std::string broadcast = "";

			while (std::getline(moduleConfig, line)) {

				auto delim1 = line.find("//");
				line = line.substr(0, delim1);
				trim(line);

				auto firstDelimiterIndex = line.find(":");
				if (firstDelimiterIndex != std::string::npos) {

					//we found a config line with a key value (and a broadcast value) so lets safe the old entry if there is one
					if (!(key == "")) {
						entries.push_back({key, value, broadcast});
					}

					auto secondDelimiterIndex = line.find(":", firstDelimiterIndex + 1);
					auto sub = line.substr(0, firstDelimiterIndex);
					key = trim(sub);
					auto sub2 = line.substr(firstDelimiterIndex + 1, secondDelimiterIndex - firstDelimiterIndex - 1);
					broadcast = trim(sub2);
					auto sub3 = line.substr(secondDelimiterIndex + 1);
					value = trim(sub3);
				} else {
					//we havent found any new config beginning so we add it to the value of the old one
					value += line;
				}
			}
			moduleConfig.close();

			//process the very last entry
			if (!(key == "")) {
				entries.push_back({key, value, broadcast});
			}
		}
		return entries;
	}

	bool starts_with(std::string &s1, const std::string &s2) {
		return (s1.find(s2) == 0);
	}

	bool ends_with(std::string &s1, const std::string &s2) {
		return (s1.find(s2) == s1.length() - s2.length());
	}

	void replace_all(std::string &s1, const std::string &s2, const std::string &s3) {
		std::size_t iter = 0;
		while ((iter = s1.find(s2, iter)) != std::string::npos) {
			s1.replace(iter, s2.length(), s3);
		}
	}

	void remove_quotes(std::string &s) {
		if (s.size() > 0 && s.at(0) == '"') {
			s = s.substr(1, s.size() - 2);
		}
	}
};

template<typename VectorType>
VectorType utils::select_random(const std::vector<VectorType>& _vec) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
	std::uniform_int_distribution<> dis(0, _vec.size() - 1);
    return _vec[dis(g)];
}

int utils::random(int max) {
	static std::random_device random_device;
	static std::mt19937 engine{ random_device() };
	std::uniform_int_distribution<int> dist(0, max);
	return dist(engine);
}

size_t utils::random(size_t max) {
    static std::random_device random_device;
    static std::mt19937 engine{ random_device() };
    std::uniform_int_distribution<size_t> dist(0, max);
    return dist(engine);
}

bool utils::iequals(const std::string& a, const std::string& b) {
	size_t sz = a.size();
	if (b.size() != sz)
		return false;
	for (size_t i = 0; i < sz; ++i)
		if (tolower(a[i]) != tolower(b[i]))
			return false;
	return true;
}

long long utils::seconds_since(const timestamp& _t)
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _t).count();
}

long long utils::minutes_since(const timestamp& _t)
{
    return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now() - _t).count();
}

long long utils::mili_seconds_since(const timestamp& _t)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _t).count();
}

template<typename T>
T* utils::pointerFromString(const std::string& _str) {
    std::stringstream _ss;
    _ss << _str;
    long long unsigned int _adr;
    _ss >> std::hex >> _adr;
    T * _data = reinterpret_cast<T *>(_adr);
    return _data;
}

template<typename T>
std::string utils::pointerToString(T* _ptr) {
    std::stringstream _ss;
    _ss << (void const *)_ptr;
    return _ss.str();
}
