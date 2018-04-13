#ifndef _WININET_COOKIE_H
#define _WININET_COOKIE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <array>

namespace toc {
namespace tuk2 {

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
  WininetCookie(const std::string& path, const std::string& domain)
      : path_(path), domain_(domain) {}

  bool isValidCookie(struct CookieInfo cookie);
  /* getter */
  std::string getPath() const { return this->path_; }
  std::string getDomain() const { return this->domain_; }

  std::vector<struct Cookie> getCookieList(); /* FIXME : shared_ptr? */
  std::string getCookies();                   /* name=data; name=data; ... */

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
  using CookieMap = std::map<std::string, WininetCookie*>;

 public:
  WininetCookieContainer() = delete;
  WininetCookieContainer(std::string& domain) : domain_(domain) {}

  std::string getDomain() const { return this->domain_; }
  WininetCookie* findCookieWithPath(const std::string& path);
  WininetCookie* createCookieWithPath(const std::string& path);

 private:
  std::string domain_;
  CookieMap path_list_;
};

class WininetCookieContainerList {
  using ContainerMap = std::map<std::string, WininetCookieContainer*>;

 public:
  static WininetCookieContainerList* GetInstance() {
    if (!instance_) {
      instance_ = new WininetCookieContainerList();
    }
    return instance_;
  }
  WininetCookieContainer* findCookieContainer(std::string host);
  WininetCookieContainer* createCookieContainer(std::string host);

 private:
  WininetCookieContainerList() {}
  bool init(); /* loading persistent cookies */

  bool initialized_;
  ContainerMap cookie_containers_;

  static WininetCookieContainerList* instance_;
};

} /* namespace tuk2 */
} /* namespace toc */

#endif /* _WININET_COOKIE_H */
