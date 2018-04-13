#include <stdio.h>

typedef int BOOL;

int main () {
	int modifier = 0x7;
	int http = 0x4;
	BOOL check1 = modifier & http;
	fprintf(stdout, "check1 : %d\n", check1);
	BOOL check2 = !check1;
	fprintf(stdout, "check2 : %d\n", check2);
	BOOL check3 = !check2;
	fprintf(stdout, "check3 : %d\n", check3);

	return 0;
}
