#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <string>

int main() {
	char name[] = "SangJun Lee";

	fprintf(stdout, "========== TEST 1 ==========\n");
	fprintf(stdout, "처음이름 : %s\n", name);
	/* lower case */
	for (int i=0; i<strlen(name); ++i) {
		char ch = (char)tolower(name[i]);
		name[i] = ch;
	}
	fprintf(stdout, "Lower Case : %s\n", name);
	for (int i=0; i<strlen(name); ++i) {
		char ch = (char)toupper(name[i]);
		name[i] = ch;
	}
	fprintf(stdout, "Upper Case  : %s\n", name);

	fprintf(stdout, "========== TEST 2 ==========\n");
	std::string name2 = "SangBin Jung";
	fprintf(stdout, "name2 : %s\n", name2.c_str());

	return 0;
}
