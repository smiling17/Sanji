#include <stdio.h>
#include "static.h"

std::string Myinfo::name_ = "sangjun";

int main() {
	int ret = -1;

	Myinfo* info = new Myinfo;
	info->getMyname();

	ret = 0;
	return ret;
}
