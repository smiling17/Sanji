#include <string>
#include <iostream>

void shirnkString(std::string& str) {
	size_t start = str.find_first_not_of(' ');
	if (start == std::string::npos) {
		return;
	}
	str = str.substr(start, str.find_last_not_of(' ') - start + 1);
}

int main() {
	std::string test1 = "   sangjun lee   ";
	std::string test2 = "sangjun lee2   ";
	std::string test3 = "   sangjun lee3";

	std::cout << "STRING BEFORE : " << test1 << "!!!" << test2 << "!!!" << test3 << "!!!" << std::endl;

	shirnkString(test1);
	shirnkString(test2);
	shirnkString(test3);

	std::cout << "STRING AFTER : " << test1 << "!!!" << test2 << "!!!" << test3 << "!!!" << std::endl;
}

