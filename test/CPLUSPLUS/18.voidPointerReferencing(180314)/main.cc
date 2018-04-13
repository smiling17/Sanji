// POSIX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * [TEST]
 * - void* 의 사용 방법 이해
 *
 * [RESULT]
 * - void* 로도 referencing 할 수 있음
 * - pointer 연산ㅇ은 할 수 없음
 */

int MemoryAlloc(void** pBuffer, unsigned int size) {
	*pBuffer = malloc(size+1);
	if (*pBuffer == nullptr) {
		return -1;
	}
	strncpy((char*)*pBuffer, "success", size);
	fprintf(stdout, "%s\n", *pBuffer);
	return 0;
}

int main() {
	void* buffer = nullptr;
	if (MemoryAlloc(&buffer, 10) != 0) {
		return -1;
	}
	fprintf(stdout, "%c\n", *buffer);
	buffer += 4;
	return 0;
}
