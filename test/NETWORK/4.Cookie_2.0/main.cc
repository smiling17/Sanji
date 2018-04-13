#include "wininet_cookie.h"

#include <iostream>

void setCookie(std::string domain, std::string path,std::string data) {
	/**
	 * SetCookie 기능
	 * 1) HOST로서 container를 찾고 -> PATH로 cookie객체를 찾아서 -> 내부 set
	 * 2) true는 없으면 생성한다는 옵션 : SET에서만 해당
	 */

	fprintf(stdout, "\n===== SET COOKIE TEST =====\n");
	WininetCookieContainer* container = WininetCookieContainer::FindCookieContainer(domain, true);
	if (container) {
		WininetCookie* cookie_path = container->findCookieWithPath(path, true);
		if (cookie_path) {
			cookie_path->setCookie(data);
		}
	}
}

std::string getCookie(std::string domain, std::string path) {
	fprintf(stdout, "===== GET COOKIE TEST =====\n");
	std::string cookie_list, path_tmp(path);
	WininetCookieContainer* container;
	while (!domain.empty() ) {
		fprintf(stdout, "===== ENTER1 =====\n");
		container = WininetCookieContainer::FindCookieContainer(domain, false);
		if (container) {
			while (!path_tmp.empty()) {
				fprintf(stdout, "===== ENTER2 =====\n");
				WininetCookie* cookie_path = container->findCookieWithPath(path_tmp, false);
				if (cookie_path) {
					fprintf(stdout, "PATH : %s\n", path_tmp.c_str());
					fprintf(stdout, "DOMAIN : %s\n", domain.c_str());
					std::string cookies = cookie_path->getCookies();
					cookie_list.append(cookies);
					fprintf(stdout, "GET COOKIE : %s\n", cookies.c_str());
				}
				path_tmp = WininetCookieContainer::getParentPath(path_tmp);
			}
		}
		fprintf(stdout, "===== EXIT 1 =====\n");
		domain = WininetCookieContainer::getParentDomain(domain);
		path_tmp = path;
	}
	return cookie_list;
}

int main () {
	std::string cookie_input = "A=B; C=D; E=F; domain=www.naver.com; path=/trunk/src/; httponly";
	std::string cookie_input2 = "G=H; domain=www.naver.com; path=/trunk/src/; httponly";
	std::string cookie_input3 = "J=K; domain=www.naver.com; path=/trunk/src/; httponly";
	std::string host_input = "www.naver.com";
	std::string path_input = "/trunk/src/bin/network/lsj.txt";
	std::string host_input2 = "naver.com";
	std::string path_input2 = "/trunk/src/bin/";
	std::string domain, path;
	path = WininetCookieContainer::getCookiePath(cookie_input); /* input으로 path 얻음(ex. /trunk/src/), class 함수 */
	domain = WininetCookieContainer::getCookieDomain(cookie_input); /* input으로 domain 얻음(ex. different.com), class 함수 */

	setCookie(domain, path, cookie_input);
	setCookie(domain, path, cookie_input2);
	setCookie(host_input2, path_input2, cookie_input3);

	std::string cookie = getCookie(host_input, path_input);
	fprintf(stdout, "FINAL COOKIE : %s\n", cookie.c_str());

	/**
	 * GetCookie 기능
	 * 1) HOST로서 container를 찾고 -> PATH로 cookie객체를 찾아서 -> 내부 set
	 * 2) false는 없으면 없다고 넘김: GET에서만 해당
	 * 3) GET을 할 때는 추후 주어진 host, path의 상위값을 얻어 for(while)로 진행
	 */
	
	fprintf(stdout, "\n===== Static Fuction Test =====\n");
	while (!host_input.empty() ){ /* empty() 활용 */
		fprintf(stdout, "host name : %s\n", host_input.c_str());
		host_input = WininetCookieContainer::getParentDomain(host_input);
	}

	while (!path_input.empty() ){
		fprintf(stdout, "path name : %s\n", path_input.c_str());
		path_input = WininetCookieContainer::getParentPath(path_input);
	}
	return 0;
}
