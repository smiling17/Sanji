#include <stdio.h>

#include <string>
#include <vector>

int main() {
	int ret = -1;
	std::string headers = "\r\nMalformedtest:value\r\n\nheader2:value2\r\n";
	std::string header;
	bool escape_r = false;

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		fprintf(stdout, "iterator test : %c\n", (*it));
		if (escape_r) {
			if ((*it) == '\n') {
				escape_r = false;
				if (header.empty()) {
					continue;
				}
				fprintf(stdout, "success!\n");
				fprintf(stdout, "header : %s\n", header.c_str());
				header.clear();
				continue;
			}
			else {
				fprintf(stdout, "invalid headers!\n");
				return ret;
			}
		}
		if ((*it) == '\r') {
			escape_r = true;
			continue;
		}
		header.append(1, *it);
//		header += *it;
	}

	ret = 0;
	return ret;
}
