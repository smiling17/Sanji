#include <string>

#include "wininet_utility.h"

namespace toc {
namespace tuk2 {

std::string WininetUtil::GetDomainFromData(std::string& data) {
  std::string domain;
  int domain_start = data.find("domain");
  if (domain_start != std::string::npos) {
    std::string data_domain = data.substr(domain_start);
    int equal = data_domain.find_first_of("=");
    int colon = data_domain.find_first_of(";");
    if (colon != std::string::npos) {
      domain = data_domain.substr(equal + 1, colon - equal - 1);
    } else {
      domain = data_domain.substr(equal + 1);
    }
    while (!domain.size() && domain.at(0) == ' ' && domain.at(0) == '.') {
      /* remove white space */
      domain = domain.substr(1);
    }
  }
  return domain;
}

std::string WininetUtil::GetPathFromData(std::string& data) {
  std::string path;
  int path_start = data.find("path");
  if (path_start != std::string::npos) {
    std::string data_path = data.substr(path_start);
    fprintf(stdout, "data_path : %s\n", data_path.c_str());
    int equal = data_path.find_first_of("=");
    int colon = data_path.find_first_of(";");
    if (colon != std::string::npos) {
      path = data_path.substr(equal + 1, colon - equal - 1);
    } else {
      path = data_path.substr(equal + 1);
    }
    while (!path.size() && path.at(0) == ' ') {
      /* remove white space */
      path = path.substr(1);
    }
  }
  return path;
}

std::string WininetUtil::GetParentDomain(std::string& url) {
  std::string parent;
  int sep = url.find_first_of(".");
  if (sep != std::string::npos) {
    parent = url.substr(sep + 1);
  }
  return parent;
}

std::string WininetUtil::GetParentPath(std::string& path) {
  std::string parent = path.substr(0, path.size() - 1); /* 맨 끝의 / 를 삭제 */
  int sep = parent.find_last_of("/");
  if (sep != std::string::npos) {
    parent = parent.substr(0, sep + 1);
  }
  return parent;
}

/* TODO : T 대신 String으로 알만한 이름 변경 */
template <typename T>
WininetUrlInfo<T>::WininetUrlInfo(T& url) {
  refreshUrl(url);
}
template <typename T>
void WininetUrlInfo<T>::refreshUrl(T url) {
  this->url_ = url;
  this->init(url);
}
template <typename T>
void WininetUrlInfo<T>::init(T url) {
  int colon = url_.find_first_of(':');
  if (colon == std::string::npos) {
    fprintf(stdout, "invalid Address\n");
    abort();
  }
  protocol_ = url_.substr(0, colon);

  // <protocol>:[//<net_loc>]/path][;<param>][?<query>][#<fragment]
  /* host name setting */
  T url_net = url_.substr(colon + 1);
  if (url_net[0] == '/' && url_net[0] == '/') {
    url_net = url_net.substr(2);
  }
  int at_sign = -1;
  if ((at_sign = url_net.find_first_of('@')) != std::string::npos) {
    T user_info = url_net.substr(0, at_sign);
    int password_start = -1;
    if ((password_start = user_info.find_first_of(':')) != std::string::npos) {
      this->user_name_ = user_info.substr(0, password_start);
      this->password_ = user_info.substr(password_start + 1);
    } else {
      this->user_name_ = user_info;
    }
    url_net = url_net.substr(at_sign + 1);
  }

  /* host / port setting */
  int host_end = -1;
  int port_start = -1;
  if ((host_end = url_net.find_first_of('/')) != std::string::npos) {
    if ((port_start = url_net.find_first_of(':', host_end)) !=
        std::string::npos) {
      this->host_name_ = url_net.substr(0, port_start);
      this->port_ = (url_net.substr(port_start + 1, host_end - port_start));
    } else {
      this->host_name_ = url_net.substr(0, host_end);
    }
    url_net = url_net.substr(host_end);
  }
  /* path/extra info */
  int extra_start = -1;
  if ((extra_start = url_net.find_first_of('?')) != std::string::npos ||
      (extra_start = url_net.find_first_of('#')) != std::string::npos) {
    this->url_path_ = url_net.substr(0, extra_start);
    this->extra_info_ = url_net.substr(extra_start);
  } else {
    this->url_path_ = url_net;
  }
}
template <typename T>
T WininetUrlInfo<T>::getHostName() {
  return this->host_name_;
}
template <typename T>
T WininetUrlInfo<T>::getProtocol() {
  return this->protocol_;
}
template <typename T>
T WininetUrlInfo<T>::getUserName() {
  return this->user_name_;
}
template <typename T>
T WininetUrlInfo<T>::getPassword() {
  return this->password_;
}
template <typename T>
T WininetUrlInfo<T>::getUrlPath() {
  return this->url_path_;
}
template <typename T>
T WininetUrlInfo<T>::getExtraInfo() {
  return this->extra_info_;
}
//template <typename T>
//unsigned int WininetUrl16<T>::getScheme() {
//	return this->scheme_;
//}
template <typename T>
T WininetUrlInfo<T>::getPort() {
  return this->port_;
}

/* currnet use for template */
template class WininetUrlInfo<std::string>;
template class WininetUrlInfo<std::u16string>;
}
}
