#include "wininet_cookie.h"
#include "../1.CrackUrl/wininet_utility.h"

#include <stdio.h>

int main() {
	fprintf(stdout, "COOKIE TEST : Set Cookie ");
	std::string url("http://www.naver.com");
	std::string data_exam("G=H; domain=.example.com; invalid=attr; path=/foo");
	WininetCookie* container = WininetCookie::getInstance(url, true);
	container->setCookie("Set Cookie1", "SangJun Lee");
	container->setCookie("Set Cookie2", "Jangwook Lee");
	container->setCookie("Set Cookie3", "GeonRyun Jo");
	container->setCookie("Set Cookie4", "SungHyun Kim");
	container->setCookies(data_exam);

	int i = 1;
	for (auto it : container->getCookie()) {
		fprintf(stdout, "===== Cookie #%d =====\n", i);
		fprintf(stdout, "name : %s\n", it.name.c_str());
		fprintf(stdout, "data : %s\n", it.data.c_str());
		++i;
	}
	fprintf(stdout, "Cookie length : %d\n", container->getCookieLength());	
	fprintf(stdout, "Cookie Output : %s\n", container->getCookies().c_str());	
	return 0;	
}
