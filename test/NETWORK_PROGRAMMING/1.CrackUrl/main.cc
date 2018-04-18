#include "wininet_utility.h"

#include <stdio.h>

#include <iostream>
#include <string>

int main() {
	std::string url = "http://sports.news.naver.com/kbaseball/news/read.nhn?oid=001&aid=0009763305";
	toc::tuk2::WininetUrl16 data(url);
	 
	std::cout << "HOST NAME : " << data.getHostName() << std::endl;
	std::cout << "PROTOCOL NAME : " << data.getProtocol() << std::endl;
	std::cout << "USER NAME : " << data.getUserName() << std::endl;
	std::cout << "PASSWORD : " << data.getPassword() << std::endl;
	std::cout << "URL PATH : " << data.getUrlPath() << std::endl;
	std::cout << "EXTRA INFO : " << data.getExtraInfo() << std::endl;
	return 0;
}
