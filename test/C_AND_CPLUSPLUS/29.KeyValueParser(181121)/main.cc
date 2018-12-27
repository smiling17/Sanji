#include <fstream> /* std::ifstream */
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#define CONFIG "/etc/default/tnetd"

class ConfigParser {
	public:
		using ConfigMap = std::map<std::string, std::string>;

		ConfigParser(const char* path) : path_(path) { loadConfig(); }
		void printParser() {
			std::cout << "PRINT START " <<  std::endl;
			for (auto& it : map_) {
				std::cout << "KEY : " << it.first << " VALUE : " << it.second << std::endl;
			}
			std::cout << "PRINT END " <<  std::endl;
		}

	private:
		void loadConfig();

	private:
		const std::string path_;
		ConfigMap map_;
};

void ConfigParser::loadConfig() {
	std::ifstream infile(path_);
	std::string line;
	map_.clear();

	int i = 0;
	while (getline(infile, line)) {
		std::cout << "NUM : " << i << " LINE : " << line << std::endl;
		++i;
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '=')) {
			std::string line_tmp;
//			is_line >> line_tmp;
//			std::cout << "is line : " << line_tmp <<  std::endl;
			std::string value;
			if (key[0] == '#') {
				std::cout << "prefix # wil be ignored, continue..." << std::endl;
				continue;
			}
			if (std::getline(is_line, value)) {
				map_[key] = value;
			}
		}
	}
}


int main () {
	ConfigParser parser(CONFIG);
	parser.printParser();
	return 0;
}
