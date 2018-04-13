#include "wininet_cookie.h"

namespace toc {
namespace tuk2 {

// static member initialization
const std::array<std::string, WininetCookie::WININET_COOKIE_END>
    WininetCookie::cookie_attrs_ = {"domain", "path", "expiration", "http_only",
                                    "secure"};
WininetCookieContainerList* WininetCookieContainerList::instance_ = nullptr;

/* WininetCookieContainerList */
WininetCookieContainer* WininetCookieContainerList::findCookieContainer(
    std::string host) {
  if (!initialized_)
    this->init();
  auto it = cookie_containers_.find(host);
  if (it == cookie_containers_.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}

WininetCookieContainer* WininetCookieContainerList::createCookieContainer(
    std::string host) {
  if (!initialized_)
    this->init();
  auto it = findCookieContainer(host);
  if (it == nullptr) {
    WininetCookieContainer* container = new WininetCookieContainer(host);
    cookie_containers_.insert(ContainerMap::value_type(host, container));
    return container;
  } else {
    return it; /* TODO : log already exist */
  }
}

bool WininetCookieContainerList::init() {
  /* TODO :loading persistent cookies */
  initialized_ = true;
  return true;
}

/* WininetCookieContainer */
WininetCookie* WininetCookieContainer::findCookieWithPath(
    const std::string& path) {
  auto it = path_list_.find(path);
  if (it == path_list_.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}

WininetCookie* WininetCookieContainer::createCookieWithPath(
    const std::string& path) {
  auto it = path_list_.find(path);
  if (it == path_list_.end()) {
    WininetCookie* cookie = new WininetCookie(path, this->getDomain());
    path_list_.insert(CookieMap::value_type(path, cookie));
    return cookie;
  } else {
    return it->second;
  }
}
/* WininetCookie */

void WininetCookie::setCookie(std::string data) {
  std::string single_data;
  std::string expiration;
  int sep = 0;
  bool http_only = false, secure = false, persistent = false,
       parsing_end = false;

  /* STEP 1 : flag check */
  if (data.find(cookie_attrs_[WININET_COOKIE_HTTPONLY]) != std::string::npos) {
    http_only = true;
  }
  if (data.find(cookie_attrs_[WININET_COOKIE_SECURE]) != std::string::npos) {
    secure = true;
  }
  if (data.find(cookie_attrs_[WININET_COOKIE_EXPIRATION]) !=
      std::string::npos) {
    persistent = true;
    expiration = getExpiratoin(data); /* TODO : 기능추가 */
  }
  /* STEP 2 : name=value cookie parsing */
  while (!parsing_end) {
    if ((sep = data.find_first_of(';')) != std::string::npos) {
      single_data = data.substr(0, sep);
    } else {
      parsing_end = true;
    }
    int equal = -1;
    std::string name, value;
    if ((equal = single_data.find_first_of('=')) != std::string::npos) {
      name = single_data.substr(0, equal);
      value = single_data.substr(equal + 1);
    } else {
      name = single_data;
    }
    data = data.substr(sep + 2);
    if (!isValidCookieName(name)) {
      continue;
    }
    /* print log */
    //    fprintf(stdout, "host: %s\n", this->getDomain().c_str());
    //    fprintf(stdout, "path : %s\n", this->getPath().c_str());
    //    fprintf(stdout, "name : %s\n", name.c_str());
    //    fprintf(stdout, "value : %s\n", value.c_str());
    removeSpace(name);
    removeSpace(value);
    struct CookieInfo cookie = {name,       value,  expiration,
                                persistent, secure, http_only};
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
  for (int i = 0; i < WININET_COOKIE_END; ++i) {
    if (name.compare(cookie_attrs_[i]) == 0) {
      return false;
    }
  }
  return true;
}

bool WininetCookie::flushPersistentCookie(struct CookieInfo cookie) {
  /* TODO : flush persistent cookie */
  return true;
}

std::string WininetCookie::getExpiratoin(std::string data) {
  std::string expiration;
  int exp_start = data.find(cookie_attrs_[WININET_COOKIE_EXPIRATION]);
  if (exp_start != std::string::npos) {
    std::string data_exp = data.substr(exp_start);
    fprintf(stdout, "data_expiration : %s\n", data_exp.c_str());
    int equal = data_exp.find_first_of("=");
    int colon = data_exp.find_first_of(";");
    if (colon != std::string::npos) {
      expiration = data_exp.substr(equal + 1, colon - equal - 1);
    } else {
      expiration = data_exp.substr(equal + 1);
    }
    removeSpace(expiration);
  }
  return expiration;
}

std::string WininetCookie::getCookies() {
  std::string cookies;
  for (auto it : this->cookie_list_) {
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

} /* namespace tuk2 */
} /* namespace toc */
