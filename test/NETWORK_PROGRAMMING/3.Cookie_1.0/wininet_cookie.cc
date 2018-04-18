#include <stdio.h>

#include "wininet_cookie.h"

std::map<std::string, WininetCookie*> WininetCookie::cookie_containers_;


/* public method */
void WininetCookie::setCookie(std::string name,	std::string value) {
	for (auto it : this->cookie_list_) {
		if (it.name.compare(name) == 0) {
			it.value = value;
			return;
		}
	}
	/* add new cookie */
	struct CookieData cookie = {name, value};
	this->cookie_list_.push_back(cookie);
	return;
}

std::vector<struct CookieData> WininetCookie::getCookie() {
	return this->cookie_list_;
}

int WininetCookie::getCookieLength() {
	/* TODO : static으로 url 받는 함수로 추가 */
	return this->calculateCookieLength();
}

/* private method */

void WininetCookie::initCookie() {
	// TODO : initialize cookie with persistent cookie file */
	return;
}

WininetCookie* WininetCookie::findCookieContainer(std::string url) {
	auto it = cookie_containers_.find(url);
	if (it == cookie_containers_.end()) {
		fprintf(stderr, "COOKIE CONTAINER NOT FOUND : %s\n", url.c_str());
		return nullptr;
	}
	return it->second;
}

int WininetCookie::calculateCookieLength() {
	int length = 0;
	for (auto it : cookie_list_) {
		length += it.name.size();
		length += it.data.size();
		length += 2; /* include ": " */
	}
	return length;
}

std::string WininetCookie::getCookies() {
	std::string cookies;
	/* etc cookies */
	for (auto it : this->getCookie()) {
		cookies.append(it.name);
		cookies.append("=");
		cookies.append(it.data);
		cookies.append("; ");
	}
	/* flags check */
	if (this->flags_ & WININET_HTTP_ONLY) {
		cookies.append("httponly; ");
	}
	if (this->flags_ & WININET_SECURE) {
		cookies.append("secure; ");
	}
	/* domain / path check */
	if (this->domain_.length()) {
		cookies.append("domain=");
		cookies.append(this->domain_);
		cookies.append("; ");
	}
	if (this->path_.length()) {
		cookies.append("path=");
		cookies.append(this->path_);
		cookies.append("; ");
	}
	/* end check : erase "; " */
	cookies = cookies.substr(0, cookies.length()-2);
	return cookies;
}
void WininetCookie::setCookies(std::string data) {
	/* reference by wine test source */
	std::string single_data;
	int sep = 0;
	int flag = 0;
	bool end = false;
	while ((sep = data.find_first_of(";")) != std::string::npos || !end) {
		single_data = data.substr(0, sep);
		fprintf(stdout, "single_data : %s\n", single_data.c_str());
		int equal = -1;
		if ((equal = single_data.find_first_of('=')) != std::string::npos) {
			std::string name = single_data.substr(0,equal);
			std::string value = single_data.substr(equal+1);
			fprintf(stdout, "name : %s\n", name.c_str());
			fprintf(stdout, "value : %s\n", value.c_str());
			/* domain / path / expiry 는 별도 설정, TODO : code 간소화  */
			if (name.compare("domain") == 0) {
				this->domain_ = value;
			}
			else if (name.compare("path") == 0) {
				this->path_ = value;
			}
			else if (name.compare("expires") == 0) {
				this->expires_ = value;
			}
			else {
				this->setCookie(name, value);
			}
		}
		else if (single_data.compare("secure") == 0) {
			flag |= WININET_SECURE;
		}
		else if (single_data.compare("http_only") == 0) {
			flag |= WININET_HTTP_ONLY;
		}
		if (sep == std::string::npos) {
			end = true;
			continue;
		}
		data = data.substr(sep+1);
		/* 함수로 별도로 빼자 */
		while (data.at(0) == ' ') {
			/* erase white space */
			data = data.substr(1); /* erase - remove 활용 */
			fprintf(stdout, "data : %s\n", data.c_str());
		}
	}
	return;
	/* TODO : validation check */
}

int WininetCookie::flushPersistentCookies() {
	if (this->expires_.length() == 0)
		return -1;
	return 0;
}
