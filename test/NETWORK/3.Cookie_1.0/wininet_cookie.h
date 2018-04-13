#ifndef _WININET_COOKIE_H
#define _WININET_COOKIE_H

#include <string>
#include <vector>
#include <map>

struct CookieData {
	std::string name;
	std::string data;
};

class WininetCookie {
	using CookieContainer = std::map<std::string, WininetCookie*>;
	enum WininetCookieFlag {
		WININET_HTTP_ONLY = 0x1,
		WININET_SECURE = 0x2,
	};
	public:
		static WininetCookie* getInstance(std::string url, bool create) {
			auto it = cookie_containers_.find(url);
			if (it == cookie_containers_.end()) {
				if (create) {
					WininetCookie* cookie_container = new WininetCookie(url);
					cookie_containers_.insert( CookieContainer::value_type(url, cookie_container) );
					return cookie_container;
				}
				else
					return nullptr;
			}
			else {
				return it->second;
			}
		}
		/* setter */
		void setCookie(std::string name, std::string value);
		void setCookies(std::string data); /* all data parsing API */
		void setFlag(WininetCookieFlag flag) { this->flags_ = flag; }
		
		/* getter */
		std::vector<struct CookieData> getCookie();
		std::string getCookies(); /* list of cookies to add libcurl/return */
		int getCookieLength();
		int getFlag() { return this->flags_; }
		std::string getDomain() { return this->domain_; }
		std::string getPath() { return this->path_; }

		/* flush */
		int flushPersistentCookies();

	private:
		WininetCookie() = delete;
		WininetCookie(const std::string& url);
		~WininetCookie() {}

		void initCookie();
		WininetCookie* findCookieContainer(std::string url);
		int calculateCookieLength();

		/* CookieList (std::map */
		static CookieContainer cookie_containers_;

		/**
		 * Default attributes
		 * 1) domain (+ sub_domain)
		 * 2) path_;
		 * 3) url
		 */
		std::string url_;
		std::string initial_domain_;
		std::string initial_path_;

		std::string domain_;
		std::string path_;
		WininetCookieFlag flags_; /* HTTP_ONLY, SECURE... */
		std::string expires_; /* DAY, DD-MMM-YYYY HH:MM:SS GMT format */
		std::vector<struct CookieData> cookie_list_; /* custom cookies */
};

#endif /* wininet_cookie.h */
