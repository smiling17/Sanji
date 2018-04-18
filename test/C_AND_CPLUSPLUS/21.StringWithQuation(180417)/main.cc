#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int is_special_char(const char input) {
	const char list[] = {'$', '\"', '\\', '`'};
	for (int i = 0; i < sizeof(list); ++i) {
		if (input == list[i]) {
			return 1;
		}
	}
	return 0;
}

int convert_string_for_escape(const char* input, unsigned int input_len,
		char* output, unsigned int output_len) {
	char *pOutput = output;
	char *random_ptr = NULL;
	fprintf(stdout, "start");
	for (int i = 0; i < input_len; ++i, ++pOutput) {
		if (is_special_char(input[i])) {
			*pOutput++ = '\\';
		}
		*pOutput = input[i];
	}
	++pOutput;
	*pOutput = '\0';
	/* TODO : validation check of length */
	return 0;
}

int main() {
	const char* input = "test\"\"12`34";
	char output[256] = {0, };
	int ret = convert_string_for_escape(input, strlen(input), output, sizeof(output));
	fprintf(stdout, "input : %s, length : %d\n" , input, strlen(input));
	fprintf(stdout, "output : %s, length : %d\n" , output, strlen(output));
	return ret;
}

