#include <iostream>
#include <string>
#include <sstream>
#include <vector>

static const char* test_string = "   iface eno1  inet   dhcp";

void printTokens(const std::vector<std::string>& tokens) {
	std::cout << "Start to print tokens" << std::endl;
	int idx = 0;
	for (auto& it : tokens) {
		std::cout << "token " << idx << " : "<< it << std::endl;
		++idx;
	}
	std::cout << "End of print tokens" << std::endl;
}

int main () {
	std::string line(test_string);
	std::stringstream line_stream(line);
	std::string token;
	std::vector<std::string> tokens;
	const char delim = ' ';
	while (std::getline(line_stream, token, delim)) {
		if (token.empty()) {
			std::cout << "continue to tokenize... : "<< token << std::endl;
			continue;
		}
		tokens.push_back(token);
	}
	printTokens(tokens);
	return 0;
}
