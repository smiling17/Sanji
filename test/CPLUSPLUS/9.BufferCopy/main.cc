#include <string>
#include <iostream>

#include <string.h>

int buffer_test() {
	char *test_buffer = "myname==sangjunlee20";
	// internetRead (handle, char** buf, length);
	// malloc(length+1);
	fprintf(stdout, "buffer length : %d\n", strlen(test_buffer));

	fprintf(stdout, "\n===== TEST1 :10만큼 카피하자 =====\n");

	char *test1 = nullptr;
	test1 = (char*)malloc(11);
	strncpy(test1, test_buffer, 10);
	fprintf(stdout, "VALUE : %s\nSIZE : %d\n", test1, strlen(test1));
	if (test1)
		free(test1);

	fprintf(stdout, "\n===== TEST2 : 10만큼 카피하자 =====\n");

	test_buffer = test_buffer + 10;
	char *test2 = nullptr;
	test2 = (char*)malloc(11);
	strncpy(test2, test_buffer, 10);
	fprintf(stdout, "VALUE : %s\nSIZE : %d\n", test2, strlen(test2));
	if (test2)
		free(test2);

	fprintf(stdout, "\n===== TEST3 : 10만큼 카피하자 =====\n");

	test_buffer = test_buffer + 10;
	char *test3 = nullptr;
	test3 = (char*)malloc(10);
	strncpy(test3, test_buffer, 10);
	fprintf(stdout, "VALUE : %s\nSIZE : %d\n", test3, strlen(test3));
	if (test3)
		free(test3);

	return 0;
}

int pointer_test() {
	int pointer_test = 0;
	int val[] = {1, 2, 3, 4, 5, 6};
	int* int_ptr = nullptr;
	int_ptr = val;
	fprintf(stdout, "첫번째값 : %d\n", *int_ptr);
	fprintf(stdout, "before move ptr : %u\n", int_ptr);
	int_ptr = int_ptr + 1;
	fprintf(stdout, "after move ptr : %u\n", int_ptr);
	fprintf(stdout, "두번째값 : %d\n", *int_ptr);
	return 0;
}

int main() {
	buffer_test();
//	pointer_test();
	return 0;
}
