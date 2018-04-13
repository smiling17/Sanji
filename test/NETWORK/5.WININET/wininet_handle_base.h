#ifndef _WININET_HANDLE_BASE_H_
#define _WININET_HANDLE_BASE_H_

#include <memory> /* smart pointer */
#include <vector>
#include <string>

#include <include/wininet.h>

/* 이를 토대로 내용 복사하기 */
#define WININET_INFO_HANDLER(func)                  \
  if (parent_.get())                                \
    return parent_.get()->func();                   \
  else {                                            \
    fprintf(stderr, "[WININET] invalid getter!\n"); \
    return nullptr;                                 \
  }

namespace toc {
namespace tuk2 {

enum class WininetHandleType {
  /* by InternetOpen() */
  WININET_UNKNOWN = -1,
  WININET_INTERNET = INTERNET_HANDLE_TYPE_INTERNET,
  /* by InternetConnect() or InternetOpenUrl() */
  WININET_CONNECT_FTP = INTERNET_HANDLE_TYPE_CONNECT_FTP,
  WININET_CONNECT_GOPHER = INTERNET_HANDLE_TYPE_CONNECT_GOPHER,
  WININET_CONNECT_HTTP = INTERNET_HANDLE_TYPE_CONNECT_HTTP,
  /* FTP specific section */
  WININET_FTP_FIND = INTERNET_HANDLE_TYPE_FTP_FIND,
  WININET_FTP_FIND_HTML = INTERNET_HANDLE_TYPE_FTP_FIND_HTML,
  WININET_FTP_FILE = INTERNET_HANDLE_TYPE_FTP_FILE,
  WININET_FTP_FILE_HTML = INTERNET_HANDLE_TYPE_FTP_FILE_HTML,
  /* Gopher specific section */
  WININET_GOPHER_FIND = INTERNET_HANDLE_TYPE_GOPHER_FIND,
  WININET_GOPHER_FIND_HTML = INTERNET_HANDLE_TYPE_GOPHER_FIND_HTML,
  WININET_GOPHER_FILE = INTERNET_HANDLE_TYPE_GOPHER_FILE,
  WININET_GOPHER_FILE_HTML = INTERNET_HANDLE_TYPE_GOPHER_FILE_HTML,
  /* HTTP specific section */
  WININET_HTTP_REQUEST = INTERNET_HANDLE_TYPE_HTTP_REQUEST,
};

template <WininetHandleType E, class WininetBaseT>
class WininetObject : public WininetBaseT {
 public:
  WininetObject() { WininetBaseT::setHandleType(E); }
  virtual ~WininetObject() {}
};

using HINTERNET = void*;

enum class WININET_ACCESS_FLAG {
  WININET_OPEN_PRECONFIG,
  WININET_OPEN_DIRECT,
  WININET_OPEN_PROXY,
  WININET_OPEN_PRECONFIG_NO_AUTOPROXY,
};

/* Info structure */
struct WininetProxyInfo {
  std::string proxy_server;
  std::string proxy_bypass;
  std::string user_name;
  std::string password;
};

/* FIXME : async type? */
struct WininetRootInfo {
	WininetRootInfo() : access_type(0), async(false) {}
	std::string agent;
	unsigned int access_type;
	bool async;
};

struct WininetConnectInfo {
	WininetConnectInfo() : server_port(0) {}
  std::string server_name;
  unsigned int server_port;
  std::string user_name;
  std::string password;
};

struct HttpRequestInfo {
	HttpRequestInfo() : status(0) {}
  unsigned int status;
  std::string verb; /* GET, POST, PUT ... */
  std::string version;
  std::string path;
};

struct HttpHeaderInfo {
  std::string field;
  std::string value;
  unsigned int flags;
  int index;
};

/*
struct HttpServerInfo {
	bool is_https;
	unsigned int port;
	std::u16string server_name;
};
*/

struct FtpFileInfo {
  std::string download_local_name;
  std::string download_remote_name;
  std::string upload_local_name;
  std::string upload_remote_name;
};

class WininetBase {
 public:
  virtual ~WininetBase() {}

  WininetHandleType getHandleType() { return handle_type_; }

  /* validation */
  bool isValid();
  void setInvalid() { valid_ = false; }
  void setIndex(const int index) {
    index_ = index;
  } /* TODO : protected로 뺄것 */

  /* common getter */
  int getIndex() const { return index_; }
  HINTERNET getHandle() const { return reinterpret_cast<HINTERNET>(index_); }

  virtual struct WininetRootInfo* getRootInfo() {
    WININET_INFO_HANDLER(getRootInfo)
  }
  virtual struct WininetProxyInfo* getProxyInfo() {
    WININET_INFO_HANDLER(getProxyInfo)
  }
  virtual struct WininetConnectInfo* getConnectInfo() {
    WININET_INFO_HANDLER(getConnectInfo)
  }
  virtual struct HttpRequestInfo* getHttpRequestInfo() {
    WININET_INFO_HANDLER(getHttpRequestInfo)
  }
  virtual struct FtpFileInfo* getFtpFileInfo() {
    WININET_INFO_HANDLER(getFtpFileInfo)
  }
  /* setter */

 protected:
  /* 외부에서는 접근 X. 상속된 객체 내에서 가능(생성자)  */
  WininetBase(WininetBase* parent = nullptr) : parent_(parent) {
    setHandleType(WininetHandleType::WININET_UNKNOWN);
  }
  void setHandleType(const WininetHandleType type) { handle_type_ = type; }

 private:
  bool valid_;
  WininetHandleType handle_type_;
  unsigned int index_; /* for handle */

  std::shared_ptr<WininetBase> parent_;
};

class WininetRootSession : public WininetBase {
 public:
  WininetRootSession() { setHandleType(WininetHandleType::WININET_INTERNET); }
  WininetRootSession(const WininetBase&) = delete;
  ~WininetRootSession() {}
  static const WininetHandleType ValidType =
      WininetHandleType::WININET_INTERNET;

  /* getter */
  virtual struct WininetRootInfo* getRootInfo() override;
  virtual struct WininetProxyInfo* getProxyInfo() override;
  INTERNET_STATUS_CALLBACK getCallback() const { return callback_; }
  unsigned int getContext() const { return context_; }

  /* setter */
  void setCallback(INTERNET_STATUS_CALLBACK callback) { callback_ = callback; }
  void setRootInfo(const struct WininetRootInfo& root_info);
  void setProxyInfo(const struct WininetProxyInfo& proxy_info);
  void setContext(const unsigned int context) { context_ = context; }
  void setRootFlag(const unsigned int flags) { root_flags_ |= flags; }

 private:
  std::shared_ptr<struct WininetRootInfo> root_info_;
  std::shared_ptr<struct WininetProxyInfo> proxy_info_;
  INTERNET_STATUS_CALLBACK callback_;
  unsigned int context_; /* async mode */
  unsigned int root_flags_;
};

/* HTTP Session */
class WininetConnectHttp : public WininetBase {
 public:
  WininetConnectHttp() = delete;
  WininetConnectHttp(WininetBase* parent) : WininetBase(parent) {}
  ~WininetConnectHttp() {}
  static const WininetHandleType ValidType =
      WininetHandleType::WININET_CONNECT_HTTP;

  /* getter */
  virtual struct WininetConnectInfo* getConnectInfo();

  /* setter */
  void setConnectInfo(const struct WininetConnectInfo& connect_info);

 private:
  std::shared_ptr<struct WininetConnectInfo> connect_info_;
};

class WininetHttpRequest : public WininetBase {
 public:
  WininetHttpRequest() = delete;
  WininetHttpRequest(WininetBase* parent)
      : WininetBase(parent), request_id_(-1) {}
  ~WininetHttpRequest() {}

  /* for Wininet API */
  int sendRequest();
  int queryResponseDataLength();
  int readData(void* buffer, int length); /* output : 실제 read 되었는지 */
  void printResponseHeader();

  /* setter */
  void addHttpHeaders(std::string headers,
                      unsigned int length,
                      unsigned int modifier);
  void setHttpRequestInfo(const struct HttpRequestInfo& request_info);
  void setHttpFlags(const unsigned int flags) { http_flags_ |= flags; }

  /* getter */
  int getRequestID() const { return request_id_; }
  std::vector<std::shared_ptr<struct HttpHeaderInfo>> getHttpHeaders();
  virtual struct HttpRequestInfo* getHttpRequestInfo() override;

 private:
  /* for addHttpHeader() */
  void modifyHttpHeader(std::string header,
                        unsigned int length,
                        unsigned int modifier);
  void insertHttpHeader(struct HttpHeaderInfo info);
  void deleteHttpHeader(struct HttpHeaderInfo info);
  void replaceHttpHeader(struct HttpHeaderInfo info);
  void coalesceHttpHeader(struct HttpHeaderInfo info);
  bool isExistHttpHeader(std::string field);

  void getIndexHttpHeader(std::string field);

  /* before HTTP request */
  void preSendRequest();
  void setRequestID(const int request_id) { request_id_ = request_id; }

  std::shared_ptr<struct HttpRequestInfo> request_info_;
  std::vector<std::shared_ptr<struct HttpHeaderInfo>> request_headers_;
  unsigned int http_flags_;
  int request_id_;
};

class WininetFtpManager;

/* WINE : ftp_session_t */
class WininetConnectFtp : public WininetBase {
 public:
  WininetConnectFtp() = delete;
  WininetConnectFtp(WininetBase* parent) : WininetBase(parent) {}
  ~WininetConnectFtp() {}
  static const WininetHandleType ValidType =
      WininetHandleType::WININET_CONNECT_HTTP;
  virtual struct WininetConnectInfo* getConnectInfo() override {
    return this->connect_info_.get();
  }
  /* setter */
  void setConnectInfo(const struct WininetConnectInfo& connect_info) { return; }
  void setConnectFlags(const unsigned int flags) { connect_flags_ |= flags; }

 private:
  std::shared_ptr<struct WininetConnectInfo> connect_info_;
  unsigned int connect_flags_;
};

/**
 * WINE : ftp_file_t
 * TODO : FTP 핸들을 따로 옮길 것인가?
 * FTP 파일 다운로드 진행중을 관리해야 함..
 */

class WininetFtpFile : public WininetBase {
 public:
  WininetFtpFile() = delete;
  WininetFtpFile(WininetBase* parent) : WininetBase(parent) {}
  ~WininetFtpFile() {}
  /* socket setting ? */
  virtual struct FtpFileInfo* getFtpFileInfo() override;

 private:
  std::shared_ptr<struct FtpFileInfo> file_info_;
  bool session_alive_;
};

#if 0
/**
 * observer pattern을 위한 class
 * 현재는 사용하지 않음
 */

class WininetObserver {
 public:
  WininetObserver() {}
  virtual ~WininetObserver() {}
  virtual void update() = 0;
};

class WininetSubject {
 public:
  virtual void notify(int index) = 0;
  virtual void attach(WininetObserver* observer) = 0;
  virtual void detach(WininetObserver* observer) = 0;

 private:
  std::vector<std::shared_ptr<WininetObserver>> parent_list_;
};
#endif
} /* namespace tuk2 */
} /* namespace toc */

#endif /*  _WININET_HANDLE_BASE_H_ */
