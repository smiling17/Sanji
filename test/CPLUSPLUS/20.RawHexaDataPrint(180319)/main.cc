#include <string>
#include <iostream>

int main() {
	char raw_data1[100] = {};
//	unsigned char* raw_data2 = {"\x0", "\x1", "\x2"}; /* error */
	char raw_data2[] = {100, 101, 102, 103, 0}; /* -128 ~ 128 */
	const char* raw_data3 = "raw_data3";

	// setting
	for (int i = 0; i < sizeof(raw_data1)/sizeof(*raw_data1); ++i) {
		raw_data1[i] = i+1;
	}
	raw_data1[99] = 0;

	// print
	fprintf(stdout, "raw_data1 : %s\n", raw_data1);
	fprintf(stdout, "raw_data2 : %s\n", raw_data2);
	fprintf(stdout, "raw_data3 : %s\n", raw_data3);

	return 0;
}
