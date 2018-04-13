#include <string>

#include <stdio.h>

int main() {
	std::string header = "namesangjun:";
	std::string field, value;
	unsigned int sep;

	if ((sep = header.find_first_of(':')) != 0) {
		fprintf(stdout, "header : %s\n", header.c_str());
		fprintf(stdout, "sep : %d\n", sep);
		field = header.substr(0, sep);
		value = header.substr(sep+1);
		if (field.find(':') || value.find(':') || (sep == header.size())) {
			fprintf(stdout, "invalid header : %s\n", header.c_str());
			return -1;
		}
		fprintf(stdout, "field : %s\n", field.c_str());
		fprintf(stdout, "value : %s\n", value.c_str());
	}
	return 0;
}
