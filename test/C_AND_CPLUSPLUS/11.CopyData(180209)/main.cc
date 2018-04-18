#include <string>
#include <iostream>

int main () {
	int ret = -1;
	char* data_ptr = nullptr;
	char data[16] = "sangjun";
	std::string data_str(data);
	std::cout << "length : " << data_str.length() << std::endl;
	return ret;
}
