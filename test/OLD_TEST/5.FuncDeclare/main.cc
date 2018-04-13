
#include "func.h"

int main() {
	int ret = -1;
	int param = 2017;
	MyTest(param);
//	MyTest2(param); /* 컴파일에러 : undefined reference to Mytest2 */
	
	ret = 0;
	return ret;
}
