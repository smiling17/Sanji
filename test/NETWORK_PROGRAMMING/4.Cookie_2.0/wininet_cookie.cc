#include "wininet_cookie.h"

//ContainerMap cookie_containers_;
std::map<std::string, WininetCookieContainer*> WininetCookieContainer::cookie_containers_;

const std::array<std::string, WininetCookie::WININET_COOKIE_END> WininetCookie::cookie_attrs_ = 
	{"domain", "path", "expiration", "http_only", "secure"};

/* WininetCookieContainer */
std::string WininetCookieContainer::getCookieDomain(std::string& data) {
	std::string domain;
	int domain_start = data.find("domain");
	if (domain_start != std::string::npos) {
		std::string data_domain = data.substr(domain_start);
		int equal = data_domain.find_first_of("=");
		int colon = data_domain.find_first_of(";");
		if (colon != std::string::npos) {
			domain = data_domain.substr(equal+1, colon-equal-1);
		}
		else {
			domain = data_domain.substr(equal+1);
		}
		while (!domain.size() && domain.at(0) == ' ' && domain.at(0) == '.') {
			/* remove white space */
			domain = domain.substr(1);
		}
	}
	return domain;
}

std::string WininetCookieContainer::getCookiePath(std::string& data) {
	std::string path;
	int path_start = data.find("path");
	if (path_start != std::string::npos) {
		std::string data_path = data.substr(path_start);
		int equal = data_path.find_first_of("=");
		int colon = data_path.find_first_of(";");
		if (colon != std::string::npos) {
			path = data_path.substr(equal+1, colon-equal-1);
		}
		else {
			path = data_path.substr(equal+1);
		}
		while (!path.size() && path.at(0) == ' ') {
			/* remove white space */
			path = path.substr(1);
		}
	}
	return path;
}

std::string WininetCookieContainer::getParentDomain(std::string& url) {
	std::string parent;
	int sep = url.find_first_of(".");
	if (sep != std::string::npos) {
		parent = url.substr(sep+1);
	}
	return parent;
}

std::string WininetCookieContainer::getParentPath(std::string& path) {
	std::string parent = path.substr(0, path.size()-1); /* 맨 끝의 / 를 삭제 */
	int sep = parent.find_last_of("/");
	if (sep != std::string::npos) {
		parent = parent.substr(0, sep+1);
	}
	return parent;
}

WininetCookie* WininetCookieContainer::findCookieWithPath(std::string path, bool create) {
	auto it = path_list_.find(path);
	if ( it == path_list_.end()) {
		if (create) {
			std::string domain = this->getDomain();
			WininetCookie* cookie_path = new WininetCookie(path, domain); 
			path_list_.insert(CookieMap::value_type(path, cookie_path) );
			return cookie_path;
		}
		else {
			return nullptr;
		}
	}
	else {
		return it->second;
	}
}

/* WininetCookie */

void WininetCookie::setCookie(std::string data) {
	std::string single_data;
	std::string expiration;
	int sep = 0;
	bool http_only = false, secure = false, persistent = false, parsing_end = false;

	/* STEP 1 : flag check */
	if (data.find(cookie_attrs_[WININET_COOKIE_HTTPONLY]) != std::string::npos) {
		http_only = true;
	}
	if (data.find(cookie_attrs_[WININET_COOKIE_SECURE]) != std::string::npos) {
		secure = true;
	}
	if (data.find(cookie_attrs_[WININET_COOKIE_EXPIRATION])!= std::string::npos) {
		persistent = true;
		expiration = getExpiratoin(data); /* TODO : 기능추가 */
	}
	/* STEP 2 : name=value cookie parsing */
	while ( !parsing_end ) {
		if ((sep = data.find_first_of(';')) != std::string::npos) {
			single_data = data.substr(0, sep);
		}
		else {
			parsing_end = true;
		}
		int equal = -1;
		std::string name, value;
		if ((equal = single_data.find_first_of('=')) != std::string::npos) {
			name = single_data.substr(0, equal);
			value = single_data.substr(equal+1);
		}
		else {
			name = single_data;
		}
		data = data.substr(sep+2);
		if (!isValidCookieName(name)) {
			continue;
		}
		/* print log */
		removeSpace(name);
		removeSpace(value);
		struct CookieInfo cookie = {name, value, expiration, persistent, secure, http_only};
		this->cookie_list_.push_back(cookie);
	}
}

bool WininetCookie::flushPersistentCookieList() {
	for (auto it : this->cookie_list_) {
		if (it.persistent) {
			flushPersistentCookie(it); /* TODO : 내부에서 시간 체크 */
			continue;
		}
	}
	return true;
}

bool WininetCookie::isValidCookie(struct CookieInfo cookie) {
	/**
	 * Validation Check
	 * 1) http only (0)
	 * 2) expiration
	 * 3) secure ( not implemented, thru HTTPS )
	 */
	if (cookie.http_only) {
		return false;
	}
	/* TODO : expiration 을 통한 시간 비교 */
	return true;
}
bool WininetCookie::isValidCookieName(const std::string& name) {
	for (int i=0; i < WININET_COOKIE_END; ++i) {
		if (name.compare(cookie_attrs_[i]) == 0) {
			return false;
		}
	}
	return true;
}

bool WininetCookie::flushPersistentCookie(struct CookieInfo cookie) {
	return true;
}

std::string WininetCookie::getExpiratoin(std::string data) {
	std::string expiration;
	int exp_start = data.find(cookie_attrs_[WININET_COOKIE_EXPIRATION]);
	if (exp_start != std::string::npos) {
		std::string data_exp = data.substr(exp_start);
		int equal = data_exp.find_first_of("=");
		int colon = data_exp.find_first_of(";");
		if (colon != std::string::npos) {
			expiration = data_exp.substr(equal+1, colon-equal-1);
		}
		else {
			expiration = data_exp.substr(equal+1);
		}
		removeSpace(expiration);
//		while (!expiration.size() && expiration.at(0) == ' ') {
//			/* remove white space */
//			expiration = expiration.substr(1);
//		}
	}
	return expiration;
}

std::string WininetCookie::getCookies() {
	std::string cookies;
	for (auto it : this->cookie_list_ ) {
		if (!isValidCookieName(it.name)) {
			continue;
		}
		cookies.append(it.name);
		cookies.append("=");
		cookies.append(it.value);
		cookies.append("; ");
		continue;
	}
	return cookies;
}

void WininetCookie::removeSpace(std::string& str) {
	size_t start = str.find_first_not_of(' ');
	if (start == std::string::npos) {
		return;
	}
	str.substr(start, str.find_last_not_of(' ') - start + 1);
}
