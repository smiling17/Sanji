#include <iostream>

int main () {
	int num = 2;
	num = num << 1;
	std::cout << "initial num : " << num << std::endl;
	num = num << 1;
	std::cout << "2nd num : " << num << std::endl;
	num = num << 1;
	std::cout << "3nd num : " << num << std::endl;
	num = num << 1;
	std::cout << "4nd num : " << num << std::endl;
	num = num >> 1;
	std::cout << "5nd num : " << num << std::endl;
	return 0;
}
