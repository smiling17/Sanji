#include <string>
#include <iostream>

std::string extract_file_name(const std::string& server) {
	auto pos_start = server.find_last_of("/");
	auto pos_end = server.find_first_of("?");
	int pos_unknown = server.find_first_of("!");
	std::cout << "pos_unknown : " << pos_unknown << std::endl;
	return ( pos_end == -1 ? server.substr(pos_start+1) : server.substr(pos_start + 1, pos_end - pos_start - 1));
}

int main() {
	const std::string server = "chalkadak/list?db=1079";
	auto file_name = extract_file_name(server);
	std::cout << "file name : " <<  file_name << std::endl;
	return 0;
}
