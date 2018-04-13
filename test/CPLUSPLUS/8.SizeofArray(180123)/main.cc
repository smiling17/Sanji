#include <string>
#include <iostream>

int main() {
	char array_char[4] = {0, };
	int array_int[5] = {0, };

	fprintf(stdout, "===== SIZEOF TEST =====\n");
	fprintf(stdout, "size of array_char : %d\n", sizeof(array_char)); 
	fprintf(stdout, "size of *array_char : %d\n", sizeof(*array_char)); 
	fprintf(stdout, "size of array_int : %d\n", sizeof(array_int)); 
	fprintf(stdout, "size of array_int : %d\n", sizeof(*array_int)); 

	return 0;
}
