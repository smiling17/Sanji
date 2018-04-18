#include <wchar.h>
#include <stdio.h>
#include <locale.h>

int main() {
	int r = 1;
	const wchar_t wchar1[] = L"test";
	wchar_t wchar2 = L'T';
	const char *char3 = "character";
//	fprintf(stdout, "===== test start =====\n");
	
	wprintf(L"wprintf : %ls\n", wchar1);
	setlocale(LC_ALL, "");
//	if (r == -1)
//		perror("wprintf error");
//
//	fprintf(stdout, "wprintf return : %d\n", r);
	fprintf(stdout, "===== test end =====\n");
	return 0;
}
