#include "wininet_handle_base.h"
#include "wininet_type.h"
#include "wininet_manager.h"
#include "include/wininet.h"

namespace toc {
namespace tuk2 {

struct WininetRootInfo* WininetRootSession::getRootInfo() {
  return root_info_.get();
}

struct WininetProxyInfo* WininetRootSession::getProxyInfo() {
  return proxy_info_.get();
}

void WininetRootSession::setRootInfo(const struct WininetRootInfo& root_info) {
  this->root_info_.reset(new struct WininetRootInfo(root_info));
}

void WininetRootSession::setProxyInfo(
    const struct WininetProxyInfo& proxy_info) {
  this->proxy_info_.reset(new struct WininetProxyInfo(proxy_info));
}

/* WininetConnectHttp */

struct WininetConnectInfo* WininetConnectHttp::getConnectInfo() {
  return this->connect_info_.get();
}

void WininetConnectHttp::setConnectInfo(
    const struct WininetConnectInfo& connect_info) {
  this->connect_info_.reset(new struct WininetConnectInfo(connect_info));
}

/* WininetHttpRequest */

void WininetHttpRequest::setHttpRequestInfo(
    const struct HttpRequestInfo& request_info) {
  this->request_info_.reset(new struct HttpRequestInfo(request_info));
}

std::vector<std::shared_ptr<struct HttpHeaderInfo>>
WininetHttpRequest::getHttpHeaders() {
  return this->request_headers_;
}

struct HttpRequestInfo* WininetHttpRequest::getHttpRequestInfo() {
  return this->request_info_.get();
}

void WininetHttpRequest::printResponseHeader() {
  HttpManager* manager = CurlHttpManager::GetInstance();
  manager->printResponseHeader();
}

int WininetHttpRequest::sendRequest() {
  HttpManager* manager = CurlHttpManager::GetInstance();
  int client = this->getRequestID();
  if (client == -1) {
	  fprintf(stdout, "[HTTP_REQUEST] create manager handle\n");
    client = manager->openSession(this);
    if (client == -1) {
      return -1;
    }
    this->setRequestID(client);
  }
  this->preSendRequest();
  fprintf(stdout, "[HTTP_REQUEST] reqeust to http manager\n");
  if ((manager->sendRequest(client)) != 0) {
    return -1;
  }
  return 0;
}

int WininetHttpRequest::queryResponseDataLength() {
  int request_id = -1;
  if ((request_id = this->getRequestID()) == -1) {
    return -1;
  } else {
    HttpManager* manager = CurlHttpManager::GetInstance();
    int available_data = manager->queryAvailableData(request_id);
    if (available_data == -1) {
      return -1;
    }
    return available_data;
  }
}

int WininetHttpRequest::readData(void* buffer, int length) {
	int read_len = -1;
	int request_id = -1;
	if ((request_id = this->getRequestID()) == -1) {
		return -1;
	} 
	HttpManager* manager = CurlHttpManager::GetInstance();
	read_len = manager->readData(request_id, buffer, length);
	return read_len;
}

void WininetHttpRequest::preSendRequest() {
  /**
	 * TODO
	 * - Additaional Header
	 * - Flag check
	 *   - INTERNET_FLAG_PRAGMA_NOCACHE
	 *   - INTERNET_FLAG_PRAGMA_NOWRITE
	 * - Host 설정: default 호스트 주소를 들고 있어야 한다. (0)
	 * - Agent info : appinfo sesssion (0)
	 * - Default verb : get (0)
	 */
  struct HttpRequestInfo* request_info = getHttpRequestInfo();
  struct WininetConnectInfo* connect_info = getConnectInfo();
  struct WininetRootInfo* root_info = getRootInfo();
  if (request_info->verb.empty()) {
    request_info->verb = "GET";
    /* GET verb 설정 */
  }
  /* host setting : 넣으면 제대로 libcurl 에서 동작을 안함  */
//  std::string server_value;
//  server_value.append(connect_info->server_name);
//  server_value.append(1, ':');
//  server_value.append(std::to_string(connect_info->server_port));
//
//  struct HttpHeaderInfo host_info;
//  host_info.field = "Host";
//  host_info.value = server_value;
//  host_info.flags = HTTP_ADDREQ_FLAG_ADD_IF_NEW | HTTP_ADDHDR_FLAG_REQ;
//  insertHttpHeader(host_info);
//
  /* agent setting */
  if (!root_info->agent.empty()) {
    struct HttpHeaderInfo agent_info = {"User-Agent", root_info->agent,
                                        HTTP_ADDREQ_FLAG_ADD_IF_NEW, -1};
    insertHttpHeader(agent_info);
  }
}

void WininetHttpRequest::addHttpHeaders(std::string headers,
                                        unsigned int length,
                                        unsigned int modifier) {
  /**
	 * Header 추가내 flang
	 * - HTTP_ADDHDR_FLAG_ADD_IF_NEW : 새로운 헤더 추가시
	 * - HTTP_ADDHDR_FLAG_REPLACE
	 *
	 * <동작 process >
	 * 1) \r\n 단위로 끊어서 header:value를 판단 (while)
	 * 2) :를 기준으로  field, value 값 분리
	 * 3) modifier(flag)에 따른 field / value 처리를 한다.
	 * 4) 세부동작 : 있는지 없는지를 판단하기 위해 idex 확인
	 *
	 * < CALESCEFLAGS >
	 *
	 * < Flag 기능 >
	 *
	 * - 헤더(구조) 추가
	 * - 있는 헤더 삭제
	 *
	 * < Wine 분석 >
	 * - 기본적으로 static value를 설정해서 각 header를 가지고 있다.
	 * - 사용함수
	 *   - HTTP_InsertCustomHeader
	 *   - HTTP_GetCustomHeaderIndex
	 *   - HTTP_DeleteCustomHeader
	 *
	 */
  std::string header;
  bool escape_r = false;
  for (auto it = headers.begin(); it != headers.end(); ++it) {
    if (escape_r) {
      if ((*it) == '\n') {
        escape_r = false;
        if (header.empty())
          continue;
        modifyHttpHeader(header, length, modifier);
        header.clear();
        continue;
      } else {
        fprintf(stdout, "invalid header! \n");
        return;
      }
    } else if ((*it) == '\r') {
      escape_r = true;
      continue;
    }
    header.append(1, *it);
    continue;
  }
}

void WininetHttpRequest::modifyHttpHeader(std::string header,
                                          unsigned int length,
                                          unsigned int modifier) {
  /* token header into field and value */
  std::string field, value;
  unsigned int sep = 0;
  bool exist = false;
  if ((sep = header.find_first_of(':')) != 0) {
    if (sep == header.size()) {
      fprintf(stdout, "invalid header : %s\n", header.c_str());
      return;
    }
    field = header.substr(0, sep);
    value = header.substr(sep + 1, header.size() - sep); /* size() == lenth() */
    if (field.find(':') || value.find(':')) {
      fprintf(stdout, "invalid header : %s\n", header.c_str());
      return;
    }
  }
  /* preprocessor of modifier */
  if (modifier & HTTP_ADDHDR_FLAG_REPLACE) {
    modifier &= ~HTTP_ADDHDR_FLAG_ADD;
  }
  /* construct header info */
  exist = isExistHttpHeader(field);
  struct HttpHeaderInfo info = {field, value, modifier, -1};
  if (modifier & HTTP_ADDREQ_FLAG_ADD)
    insertHttpHeader(info);
  if (exist) {
    /* valid flag when header is existed : ADD_IF_NEW, REPLACE_ COALESCE */
    if (modifier & HTTP_ADDREQ_FLAG_ADD_IF_NEW) {
      fprintf(stdout, "invalid header\n");
      return;
    }
    if (modifier & HTTP_ADDREQ_FLAG_REPLACE) {
      replaceHttpHeader(info);
    } else if ((modifier & HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA) ||
               (modifier & HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON)) {
      coalesceHttpHeader(info);
    }
  }
}
void WininetHttpRequest::insertHttpHeader(struct HttpHeaderInfo info) {
  /**
	 * < error check >
	 * 1) already exist
	 */
  if (isExistHttpHeader(info.field))
    return;
  std::shared_ptr<struct HttpHeaderInfo> header(new HttpHeaderInfo(info));
  request_headers_.push_back(header);
}

void WininetHttpRequest::deleteHttpHeader(struct HttpHeaderInfo info) {
  for (auto& it : this->request_headers_) {
    if (it.get()->field.compare(info.field) == 0) {
      fprintf(stdout, "Ready to reset %s\n", info.field.c_str());
      it.reset(); /* index는 유지하자 */
    }
  }
}

bool WininetHttpRequest::isExistHttpHeader(std::string field) {
  for (auto& it : this->request_headers_) {
    if (it.get()->field.compare(field) == 0)
      return true;
  }
  return false;
}

void WininetHttpRequest::replaceHttpHeader(struct HttpHeaderInfo info) {
  for (auto& it : this->request_headers_) {
    if (it.get()->field.compare(info.field) == 0) {
      it.get()->value = info.value;
      it.get()->flags = info.flags;
      return;
    }
  }
  return;
}

void WininetHttpRequest::coalesceHttpHeader(struct HttpHeaderInfo info) {
  for (auto& it : this->request_headers_) {
    std::string div;
    if (it.get()->field.compare(info.field) == 0) {
      if (info.flags & HTTP_ADDHDR_FLAG_COALESCE_WITH_COMMA) {
        div = ',';
      } else if (info.flags & HTTP_ADDHDR_FLAG_COALESCE_WITH_SEMICOLON) {
        div = ';';
      }
    }
    it.get()->value.append(div);
    it.get()->value.append(' ', 1);
    it.get()->value.append(info.value);
  }
}

} /* namespace tuk2 */
} /* namespace toc */
