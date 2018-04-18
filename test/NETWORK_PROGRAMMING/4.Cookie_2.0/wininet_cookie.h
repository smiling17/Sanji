#ifndef _WININET_COOKIE_H
#define _WININET_COOKIE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <array>

struct CookieInfo {
	std::string name;
	std::string value;
	std::string expiration;
	bool persistent;
	bool secure;
	bool http_only;
};

class WininetCookie {
	public:
		/* Constructor */
		WininetCookie() = delete;
		WininetCookie(std::string& path, std::string& domain) 
			: path_(path), domain_(domain) {}

		bool isValidCookie(struct CookieInfo cookie);
		/* getter */
		std::string getPath() const { return this->path_; }
		std::string getDomain() const { return this->domain_; }

		std::vector<struct Cookie> getCookieList(); /* FIXME : shared_ptr? */
		std::string getCookies(); /* name=data; name=data; ... */

		/* setter */
		void setCookie(struct CookieInfo);
		void setCookie(std::string data);

		/* flush */
		bool flushPersistentCookieList();

	private:
		enum WININET_COOKIE_ATTRIBUTE {
			WININET_COOKIE_DOMAIN = 0,
			WININET_COOKIE_PATH,
			WININET_COOKIE_EXPIRATION,
			WININET_COOKIE_HTTPONLY,
			WININET_COOKIE_SECURE,
			WININET_COOKIE_END,
		};
		bool flushPersistentCookie(struct CookieInfo cookie);
		std::string getExpiratoin(std::string data);
		bool isValidCookieName(const std::string& name);
		void removeSpace(std::string& str);

		std::string path_;
		std::string domain_;
		std::vector<struct CookieInfo> cookie_list_;
		static const std::array<std::string, WININET_COOKIE_END> cookie_attrs_;
};

class WininetCookieContainer {
	using ContainerMap = std::map<std::string, WininetCookieContainer*>;
	using CookieMap = std::map<std::string, WininetCookie*>;

	public:
		/* for setting cookie */
		static std::string getCookieDomain(std::string& data);
		static std::string getCookiePath(std::string& data);

		/* for getting cookie */
		static std::string getParentDomain(std::string& url);
		static std::string getParentPath(std::string& path);

		static WininetCookieContainer* FindCookieContainer(std::string host, bool create) {
			/* set : Create / get : Crate Off */
			auto it = cookie_containers_.find(host);
			if ( it == cookie_containers_.end()) {
				if (create) {
					WininetCookieContainer *container = new WininetCookieContainer(host);
					cookie_containers_.insert( ContainerMap::value_type(host, container) );
					return container;
				}
				else
					return nullptr;
			}
			else {
				return it->second;
			}
		}
		std::string getDomain() { return this->domain_; } 
		WininetCookie* findCookieWithPath(std::string path, bool create);

	private:
		WininetCookieContainer() = delete;
		WininetCookieContainer(std::string& domain) : domain_(domain) {}

		std::string domain_;

		static ContainerMap cookie_containers_;
		CookieMap path_list_;
};

#endif /* _WININET_COOKIE_H */
