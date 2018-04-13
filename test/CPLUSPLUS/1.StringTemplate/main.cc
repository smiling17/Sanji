#include <stdio.h>

#include <iostream>

#include "wininet_utility.h"

int main() {
	std::string url_origin = "https://www.mattcutts.com/blog/seo-advice-url-canonicalization/";
	std::u16string url_origin_16 = u"https://www.mattcutts.com/blog/seo-advice-url-canonicalization/";
	WininetUrl16<std::string> url(url_origin);
	WininetUrl16<std::u16string> url_16(url_origin_16);
	std::cout << "HOST NAME : " << url.getHostName() << std::endl;	
	std::cout << "USER NAME : " << url.getUserName() << std::endl;	
	std::cout << "PASSWORD : " << url.getPassword() << std::endl;	
	std::cout << "PROTOCOL : " << url.getProtocol() << std::endl;	
	std::cout << "URL PATH  : " << url.getUrlPath() << std::endl;	
	return 0;
}
