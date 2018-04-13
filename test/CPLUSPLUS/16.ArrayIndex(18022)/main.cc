#include <string>
#include <iostream>
#include <vector>

int main() {
	int int_array[4] = {1, 2, 3, 4};
	fprintf(stdout, "===== test start =====\n");
		fprintf(stdout, "[TEST0] param of array name is %d\n\n", *int_array);
	for (int n = 0; n < sizeof(int_array)/sizeof(*int_array); ++n) {
		fprintf(stdout, "[TEST1] param of index %d is %d\n", n, int_array[n]);
		fprintf(stdout, "[TEST2] param of index %d is %d\n", n, *(int_array+n));
		fprintf(stdout, "[TEST3] address of index %d is %x\n", n, (int_array+n));
	}
	return 0;
}
