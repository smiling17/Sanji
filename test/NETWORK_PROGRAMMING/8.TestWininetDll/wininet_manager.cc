#include "wininet_manager.h"

#include <unistd.h>
#include <iostream>

#include <curl/curl.h>

namespace toc {
namespace tuk2 {

int CurlHttpManager::current_client_ = -1;

static int callback_count = 1;

std::map<std::string, std::string> CurlHttpManager::response_header_;
std::vector<CurlResponseData> CurlHttpManager::response_data_;
CurlHttpManager* CurlHttpManager::instance_ = nullptr; /* initialization */

/* CurlHttpManager */
CurlHttpManager* CurlHttpManager::GetInstance() {
  if (instance_ == nullptr)
    instance_ = new CurlHttpManager();
  return instance_;
}

/* Interface */
int CurlHttpManager::openSession(WininetHttpRequest* info) {
  struct CurlWininetClientInfo* client_info =
      new struct CurlWininetClientInfo();
  int id = this->clients_.size();
  client_info->cli_info = info;
  if (!this->curlInit(client_info)) {
    return -1;
  }
  this->clients_.push_back(client_info);
  return id;
}

int CurlHttpManager::closeSession(int client) {
  if (client >= this->clients_.size())
    return -1;
  struct CurlWininetClientInfo* client_info = clients_.at(client);
  if (client_info->curl_session) {
    curl_easy_cleanup(client_info->curl_session);
    client_info->curl_session = nullptr;
  }
  client_info->cli_info = nullptr;
  client_info->isValid = false;
  return 0;
}

int CurlHttpManager::setConnectInfo(struct CurlWininetClientInfo* client_info) {
  struct WininetRootInfo* root_info = client_info->cli_info->getRootInfo();
  if (root_info) {
    if (!root_info->agent.empty()) {
		fprintf(stdout, "[CURL_SET] agent setting\n");
      curl_easy_setopt(client_info->curl_session, CURLOPT_USERAGENT,
                       root_info->agent.c_str());
    }
  }
  struct WininetConnectInfo* connect_info =
      client_info->cli_info->getConnectInfo();
  if (connect_info) {
    /* HTTP / HTTPS : default */
    curl_easy_setopt(client_info->curl_session, CURLOPT_PROTOCOLS,
                     CURLPROTO_HTTP | CURLPROTO_HTTPS);
    if (!connect_info->server_name.empty()) {
		fprintf(stdout, "[CURL_SET] url setting\n");
      curl_easy_setopt(client_info->curl_session, CURLOPT_URL,
                       connect_info->server_name.c_str());
    } else {
      fprintf(stderr, "[WININET] there is no host name in the handle\n");
      return -1;
    }
    if (connect_info->server_port) {
		fprintf(stdout, "[CURL_SET] server port : %d\n", connect_info->server_port);
      curl_easy_setopt(client_info->curl_session, CURLOPT_PORT,
                       connect_info->server_port);
    }
    curl_easy_setopt(client_info->curl_session, CURLOPT_TCP_KEEPALIVE,
                     1L); /* FIXME */
  } else {
    fprintf(stderr, "[WININET] there is no connect info\n");
    return -1;
  }
  return 0;
}

int CurlHttpManager::setHttpRequestInfo(
    struct CurlWininetClientInfo* client_info) {
  struct HttpRequestInfo* request_info =
      client_info->cli_info->getHttpRequestInfo();
  if (request_info) {
    /* FIXME : 함수로 뺄지 검토 */
    if (request_info->verb.compare("GET") == 0) {
      curl_easy_setopt(client_info->curl_session, CURLOPT_HTTPGET, 1L);
    } else if (request_info->verb.compare("POST") == 0) {
      fprintf(stderr, "POST verb is not tested yet\n");
      return -1;
      //		curl_easy_setopt(client_info->curl_session, CURLOPT_HTTPGET, 1L);
    } else {
      curl_easy_setopt(client_info->curl_session, CURLOPT_HTTPGET, 1L);
    }
  }
  return 0;
}

int CurlHttpManager::setHttpRequestHeaders(
    struct CurlWininetClientInfo* client_info) {
  std::vector<std::shared_ptr<struct HttpHeaderInfo>> request_headers =
      client_info->cli_info->getHttpHeaders();
  if (request_headers.empty()) {
    fprintf(stderr, "[WININET] No Request Header\n");
    return 0;
  } else {
    fprintf(stderr, "[WININET] Request Header Found\n");
    if (client_info->curl_headers) {
      /* initialization */
      curl_slist_free_all(client_info->curl_headers);
    }
    for (auto it : request_headers) {
      if (it.get()->field.size() && it.get()->value.size()) {
        std::string custom_header;
    	fprintf(stderr, "[WININET] Field : %s\n", it.get()->field.c_str());
    	fprintf(stderr, "[WININET] Value : %s\n", it.get()->value.c_str());
        custom_header.append(it.get()->field);
        custom_header.append(": ");
        custom_header.append(it.get()->value);
        client_info->curl_headers =
            curl_slist_append(client_info->curl_headers, custom_header.c_str());
      }
    }
    CURLcode res;
    res = curl_easy_setopt(client_info->curl_session, CURLOPT_HTTPHEADER,
                           client_info->curl_headers);
    return res;
  }
}

int CurlHttpManager::sendRequest(int client) {
  /**
	 * Fuction Detail
	 * 1) HTTP Header setting (struct curl_slist, curl_slist_append)
	 * 2) Send Request (To server)
	 */
  if (client >= this->clients_.size())
    return -1;
  struct CurlWininetClientInfo* client_info = this->clients_.at(client);
  if (this->setConnectInfo(client_info) != 0) {
    return -1;
  }
  if (this->setHttpRequestInfo(client_info) != 0) {
    return -1;
  }
  if (this->setHttpRequestHeaders(client_info) != 0) {
    return -1;
  }
  fprintf(stdout, "[WININET_MANAGER] ready to set callback\n");
  /* perform */
  this->setWriteCallbackForCurl(client_info);
  this->current_client_ = client;
  struct WininetRootInfo* root_info = client_info->cli_info->getRootInfo();
  if (root_info) {
	  if (root_info->async) {
		  /* aysnc thread */
		  fprintf(stdout, "[WININET_MANAGER] aysnc process\n");
		  client_info->async_thread = std::thread( [=] {curlExec(client_info); });
		  /* TODO : join(), lock() */
		  return 0;
	  }
  }
  fprintf(stdout, "[WININET_MANAGER] ready to send\n");
  return this->curlExec(client_info);
}

int CurlHttpManager::queryAvailableData(int client) {
  int available_data = -1;
  if (current_client_ != client)
    return -1;

  for (auto& it : response_data_) {
    if (it.data_len == it.read_len)
      continue;
	/* 첫번째 것만 날려준다 */
    available_data = it.data_len;
	break;
  }
  return available_data;
}

int CurlHttpManager::readData(int client, void* buffer, int length) {
	int buffer_len = length; 
	std::string read_data;
	if (current_client_ != client) {
		return -1;
	}
	for (auto& it : response_data_) {
		if (it.data_len == it.read_len || it.data == nullptr) {
			continue;
		}
		std::string data(it.data);
		int remain_len = it.data_len - it.read_len;
		fprintf(stdout, "[MANAGER] remain length = %d\n", remain_len);
		fprintf(stdout, "[MANAGER] data length = %d\n", it.data_len);
		fprintf(stdout, "[MANAGER] read length = %d\n", it.read_len);
		if (remain_len >= length) {
			read_data.append(data, it.read_len, length);
			it.read_len += length;
			break;
		}
		else {
			read_data.append(data, it.read_len, remain_len);
			it.read_len += remain_len;
			length -= remain_len;
			continue;
		}
	}
	/* copy */
	read_data.copy(reinterpret_cast<char*>(buffer), buffer_len);
	return read_data.length();
}

int CurlHttpManager::setWriteCallbackForCurl(
    struct CurlWininetClientInfo* client_info) {
  curl_easy_setopt(client_info->curl_session, CURLOPT_WRITEFUNCTION,
                   CurlHttpManager::curl_callback);
  return 0;
}

size_t CurlHttpManager::header_callback(void* contents,
                                      size_t size,
                                      size_t nmemb,
                                      void* userp) {
	std::string header_data = (char*)contents;
	int pos;
	if ((pos = header_data.find_first_of(':')) != std::string::npos) {
		std::string key = header_data.substr(0, pos);
		std::string value = header_data.substr(pos+2);
		std::cout << "key :" << key << " value :" << value << std::endl;
		response_header_.insert(std::pair<std::string, std::string>(key, value));
	}
	return nmemb * size;
}

size_t CurlHttpManager::curl_callback(void* contents,
                                      size_t size,
                                      size_t nmemb,
                                      void* userp) {
	fprintf(stdout, "[CURL_CALLBACK] callback start : %d\n", callback_count++);
//	sleep(1);
  size_t realsize = size * nmemb;
  struct CurlResponseData data = {0, };
  data.data_len = realsize;
  data.data = static_cast<char*>(contents);
  response_data_.push_back(data);
  return realsize;
}

bool CurlHttpManager::curlInit(struct CurlWininetClientInfo* client_info) {
  this->curlClean(client_info);
  client_info->curl_session = curl_easy_init();
  if (client_info->curl_session == nullptr) {
    fprintf(stderr, "curl create fail\n");
    return false;
  }
  return true;
}
void CurlHttpManager::curlClean(struct CurlWininetClientInfo* client_info) {
  if (client_info->curl_session) {
    curl_easy_cleanup(client_info->curl_session);
    client_info->curl_session = nullptr;
  }
  if (client_info->curl_headers) {
    curl_slist_free_all(client_info->curl_headers);
    client_info->curl_headers = nullptr;
  }
}
int CurlHttpManager::curlExec(struct CurlWininetClientInfo* client_info) {
  CURLcode res;
  curl_easy_setopt(client_info->curl_session, CURLOPT_HEADERFUNCTION, CurlHttpManager::header_callback);
  fprintf(stdout, "[CURL] perform start\n");
//  curl_easy_setopt(client_info->curl_session, CURLOPT_VERBOSE, 1L);
  res = curl_easy_perform(client_info->curl_session);
  fprintf(stdout, "[CURL] perform finish\n");
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  return res;
}
} /* namespace tuk2 */
} /* namespace toc */
