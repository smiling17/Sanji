#ifndef _WININET_MANAGER_H_
#define _WININET_MANAGER_H_

#include <string>
#include <thread>
#include <map>
#include <iostream>

/* libcurl */
#include <curl/curl.h>

#include "wininet_handle_base.h"

namespace toc {
namespace tuk2 {

enum class ManagerType {
  /* libcurl 이외의 타입으로 구현할 시 */
  DEFAULT,
  LIBCURL,
};

enum QUERY_OPT {
  /* TODO : QueryOption 에 쓸 enum */
  DEFAULT = 0,
};

class HttpManager {
 public:
  virtual ~HttpManager() {}

  virtual int openSession(WininetHttpRequest* info) = 0;
  virtual int closeSession(int client) = 0;
  virtual int sendRequest(int client) = 0;
  virtual int queryAvailableData(int client) = 0;
  virtual int refreshInfo(int client) = 0;
  virtual int queryResponseInfo(int client, QUERY_OPT option) = 0;
  virtual int readData(int client, void* buffer, int length) = 0;
  virtual void printResponseHeader() = 0;
  /// @brief init config for Http request, 함수 하나에서 구현 */

 protected:
  HttpManager() {}

 private:
  /**
   * @todo connection pool setting
   */
};

struct CurlResponseData {
  unsigned int data_len;
  unsigned int read_len;
  char* data;
};

struct CurlWininetClientInfo {
	/* For Wininet */
  bool isValid;
  int cli_id;
  WininetHttpRequest* cli_info;
  /* For libCurl */
  CURL* curl_session;
  struct curl_slist* curl_headers;
  std::thread async_thread;
  std::vector<struct CurlResponseData> response_data;
};

class CurlHttpManager : public HttpManager {
 public:
  static CurlHttpManager* GetInstance();

  virtual int openSession(WininetHttpRequest* info) override;
  virtual int closeSession(int client) override;
  virtual int sendRequest(int client) override;
  virtual int queryAvailableData(int client) override;
  virtual int refreshInfo(int client) override { return 0; }
  virtual int queryResponseInfo(int client, QUERY_OPT option) override;
  virtual int readData(int client,
		  				void* buffer,
						int length) override;
  void printResponseHeader() override {
	  for (auto it = response_header_.begin(); it != response_header_.end(); ++it) {
		  std::cout << it->first << " => " << it->second << "value size : " << it->second.length() << std::endl;
	  }
  }
 private:
  /**
		 * @brief HTTP function / attr. for LIBCURL
		 */
  bool curlInit(struct CurlWininetClientInfo* client_info);
  /**
		 * @brief clean curl data (curl_session / curl/headers)
		 * with curl_easy_cleanup() / curl_slist_free_all()
		 */
  void curlClean(struct CurlWininetClientInfo* client_info);
  /**
		 * @brief Perform http request with curl_session
		 */
  int curlExec(struct CurlWininetClientInfo* client_info);

  /* Before sendRequest() */
  int setConnectInfo(struct CurlWininetClientInfo* client_info);
  int setHttpRequestInfo(struct CurlWininetClientInfo* client_info);
  int setHttpRequestHeaders(struct CurlWininetClientInfo* client_info);
  int setWriteCallbackForCurl(struct CurlWininetClientInfo* client_info);

  static size_t curl_callback(void* contents,
                              size_t size,
                              size_t nmemb,
                              void* userp);
  static size_t header_callback(void* contents,
                              size_t size,
                              size_t nmemb,
                              void* userp);
 private:
  ~CurlHttpManager() {}
  CurlHttpManager() {}

  std::vector<struct CurlWininetClientInfo*> clients_;

  static CurlHttpManager* instance_;
  static int current_client_;
  static std::vector<CurlResponseData> response_data_;
  static std::map<std::string, std::string> response_header_;
};

#if 0
class FactoryManager {
 public:
  /* Factory Class, include both FTP & HTTP */
  static WininetHttpManager* CreateHttpManager(ManagerType type) {
    WininetHttpManager* http_manager;
    switch (type) {
      case DEFAULT:
      case LIBCURL:
        http_manager = new CurlHttpManager();
        break;
    }
    return http_manager;
  }
  static WininetFtpManager* CreateFtpManager(FtpType type) {
    WininetFtpManager* ftp_manager;
    switch (type) {
      case DEFAULT:
      case LIBCURL:
        ftp_manager = new CurlFtpManager();
        break;
    }
    /* TODO : 초기 connection 정보 넘겨주기 */
    return ftp_manager;
  }
};

/* FTP Manager */
class WininetFtpManager {
 public:
  /* METHOD definition */
  virtual int connectToHost() = 0;
  virtual int setCurrentDir() = 0;
  virtual int uploadFile() = 0;
  virtual int downloadFile() = 0;
  virtual ~WininetFtpManager() {}

 protected:
  WininetFtpManager() {}
};
#endif

} /* namespace tuk2 */
} /* namespace toc */

#endif /* _WININET_MANAGER_H_ */
