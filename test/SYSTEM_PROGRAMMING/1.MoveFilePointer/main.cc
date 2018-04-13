#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 256
int main() {
	fprintf(stdout, "===== Move File Pointer Test =====\n");
	FILE *fp = nullptr;
	char buf[BUF_SIZE] = {0, };
	bool offset_changed = false;
	int count = 0;
	fp = fopen("test.txt", "r");
	if (fp == nullptr) {
		perror("fopen: text.txt");
		exit(1);
	}
	while (fgets(buf, BUF_SIZE, fp) != nullptr) {
		fprintf(stdout, "%s\n", buf);
		if (count == 3 && !offset_changed) {
			fseek(fp, 0, SEEK_SET);
			count = 0;
			offset_changed = true;
			continue;
		}
		count++;
	}
	fprintf(stdout, "===== Move File Pointer Test End =====\n");
	return 0;
}
