#include <string>
#include <iostream>
#include <vector>

#define IS_TMAX 0
#define IS_SK1 1
#define IGNORE 0

int main() {
	std::cout << "===== MACRO TEST =====\n";
#if !IS_TMAX
	std::cout << "I'm not Tmax\n";
#elif IS_SK1
	std::cout << "I'm SK1-1\n";
#else
	std::cout << "I'm Tmax\n";
#endif
	return 0;
}
