#include <string>

#include <stdio.h>

struct MyInfo {
	std::string name;
	std::string company;
	std::string school;
};

int main () {
	struct MyInfo info1 = {"sangjun lee", "TMAX", "POHANG UNIVERSITY"};
	struct MyInfo info2 = {"상준", "티맥스", "포항공대"};

	fprintf(stdout, "Size of info1 = %d\n", sizeof(info1));
	fprintf(stdout, "Size of info2 = %d\n", sizeof(info2));
	return 0;
}
