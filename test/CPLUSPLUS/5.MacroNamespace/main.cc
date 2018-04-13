#include "wininet_common.h"
#include <string>

int main() {
	std::string convert = "TUK_FUNCTION(BOOL WINPAI, HttpAddRequestHEaderW, HttpRequest, lpszHeader, dwHeaderLength, dwModifier)";
	fprintf(stdout, "===== CONVERTING TEST =====\n");
	fprintf(stdout, "func before : HttpAddRequestHeader\n");
	fprintf(stdout, "func after : %s\n", convert.c_str());
	fprintf(stdout, "===== CONVERTING END =====\n");
	
}
