#include <string>
#include <iostream>

int main() {
	unsigned long data1 = 2295949829;
	unsigned short data2 = 61842;
	unsigned short data3 = 4564;
	unsigned short data4[8] = {166, 95, 0, 64, 150, 50, 81 ,229};
	fprintf(stdout, "data1 : %x\n", data1);
	fprintf(stdout, "data2 : %x\n", data2);
	fprintf(stdout, "data3 : %x\n", data3);
	for (int i=0; i < sizeof(data4) / sizeof(*data4); ++i) {
		fprintf(stdout, "data4[i] : %x\n", data4[i]);
	}
	return 0;
}
