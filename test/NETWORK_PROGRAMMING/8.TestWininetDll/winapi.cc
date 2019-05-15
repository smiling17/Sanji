/**
 * @file    wininet.h
 * @brief   TODO brief documentation here.
 *
 * @history
 */
#include <string>

#include <stdio.h>
#include <string.h>

#include "include/wininet.h"

#include "winapi.h"
#include "wininet_handle_base.h"
#include "wininet_handle_table.h"
#include "wininet_manager.h"
#include "wininet_utility.h"
#include "wininet_cookie.h"

#include "lib/core/string/encoding_converter.h"

namespace toc {
namespace winapi {

using namespace tuk2;
using namespace core;

/* PART1 : HTTP Fuction */

BOOL WINAPI HttpAddRequestHeadersW(HINTERNET hHttpRequest,
                                   LPCWSTR lpszHeader,
                                   DWORD dwHeaderLength,
                                   DWORD dwModifier) {
  /* 현재 setting 되어있는 헤더
	 * 1) 핸들에서 자료구조를 받아온다. (by hHttpRequest)
	 * 2) 설정한 해더를 기반으로 헤더를 구성한다
	 * 3) 핸들값 안에 핸들을 설정한다.
	 */
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetHttpRequest* request = static_cast<WininetHttpRequest*>(
      handle_table->getObjectFromHandle(hHttpRequest));
  if (!request) {
    return false;
  }
  std::string headers =
      UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszHeader));
  unsigned int length = headers.length();
  request->addHttpHeaders(headers, length, dwModifier);
  return true;
}

BOOL WINAPI HttpEndRequestW(HINTERNET hRequest,
                            LPINTERNET_BUFFERSW lpBuffersOut,
                            DWORD dwFlags,
                            DWORD_PTR dwContext) {
  return false;
}

BOOL WINAPI HttpQueryInfoW(HINTERNET hHttpRequest,
                           DWORD dwInfoLevel,
                           LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength,
                           LPDWORD lpdwIndex) {
  return true;
}

BOOL WINAPI HttpSendRequestExW(HINTERNET hRequest,
                               LPINTERNET_BUFFERSW lpBuffersIn,
                               LPINTERNET_BUFFERSW lpBuffersOut,
                               DWORD dwFlags,
                               DWORD_PTR dwContext) {
  return true;
}

BOOL WINAPI HttpSendRequestW(HINTERNET hHttpRequest,
                             LPCWSTR lpszHeaders,
                             DWORD dwHeaderLength,
                             LPVOID lpOptional,
                             DWORD dwOptionalLength) {
  /* 함수로 통일하자 (공통된 기능) */
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetHttpRequest* request = static_cast<WininetHttpRequest*>(
      handle_table->getObjectFromHandle(hHttpRequest));
  /**
	 * TODO
	 * - agent 정보
	 * - flag 체크
	 *   - INTERNET_FLAG_PRAGMA_NOCACHE
	 *   - INTERNET_FLAG_NO_CACHE_WRITE
	 * - additional head
	 */
  if (request->sendRequest() != 0) {
    return false;
  }
  return true;
}

/* PART2 : URL Cache Fuction */

BOOL WINAPI DeleteUrlCacheEntryW(LPCWSTR lpszUrlName) {
  return true;
}

BOOL WINAPI FindCloseUrlCache(HANDLE hEnumHandle) {
  return true;
}

INTERNETAPI HANDLE WINAPI
FindFirstUrlCacheEntryW(LPCWSTR lpszUrlSearchPattern,
                        LPINTERNET_CACHE_ENTRY_INFOW lpFirstCacheEntryInfo,
                        LPDWORD lpdwFirstCacheEntryInfoBufferSize);

BOOL WINAPI
FindNextUrlCacheEntryW(HANDLE hEnumHandle,
                       LPINTERNET_CACHE_ENTRY_INFOW lpNextCacheEntryInfo,
                       LPDWORD lpdwNextCacheEntryInfoBufferSize) {
  return true;
}

/* <PART3> Internet Common */

DWORD WINAPI InternetAttemptConnect(DWORD dwReserved);

BOOL WINAPI InternetAutodial(DWORD dwFlags, HWND hwndParent) {
  return false;
}

DWORD WINAPI InternetSetFilePointer(HINTERNET hFile,
                                    LONG lDistanceToMove,
                                    PVOID pReserved,
                                    DWORD dwMoveContext,
                                    DWORD_PTR dwContext) {
  return 0;
}

BOOL WINAPI InternetCanonicalizeUrlW(LPCWSTR lpszUrl,
                                     LPWSTR lpszBuffer,
                                     LPDWORD lpdwBufferLength,
                                     DWORD dwFlags) {
  /* Input 으로 들어온 URL을 나름의 flag내에서 변형 */
  return true;
}
BOOL WINAPI InternetCrackUrlA(const char* lpszUrl,
                              DWORD dwUrlLength,
                              DWORD dwFlags,
                              URL_COMPONENTSA* lpUC) {
  if (!lpszUrl || !*lpszUrl || !lpUC) {
    return FALSE;
  }
  std::string url_convert = reinterpret_cast<const char*>(lpszUrl);
  WininetUrlInfo<std::string> url_crack(url_convert);
  /**
   * TODO
   * 1) flag 처리
   * 2) error 처리 : return FALSE;
   * 3)
   */
  lpUC->dwSchemeLength = url_crack.getProtocol().length();
  url_crack.getProtocol().copy(reinterpret_cast<char*>(lpUC->lpszScheme),
                               lpUC->dwSchemeLength);
  lpUC->dwHostNameLength = url_crack.getHostName().length();
  url_crack.getHostName().copy(reinterpret_cast<char*>(lpUC->lpszHostName),
                               lpUC->dwHostNameLength);
  lpUC->dwUserNameLength = url_crack.getUserName().length();
  url_crack.getUserName().copy(reinterpret_cast<char*>(lpUC->lpszUserName),
                               lpUC->dwUserNameLength);
  lpUC->dwPasswordLength = url_crack.getPassword().length();
  url_crack.getPassword().copy(reinterpret_cast<char*>(lpUC->lpszPassword),
                               lpUC->dwPasswordLength);
  lpUC->dwUrlPathLength = url_crack.getUrlPath().length();
  url_crack.getUrlPath().copy(reinterpret_cast<char*>(lpUC->lpszUrlPath),
                              lpUC->dwUrlPathLength);
  lpUC->dwExtraInfoLength = url_crack.getExtraInfo().length();
  url_crack.getExtraInfo().copy(reinterpret_cast<char*>(lpUC->lpszExtraInfo),
                                lpUC->dwExtraInfoLength);
  /* TODO : SCHEME 및 이를 기반한 PORT 설정 */
  return TRUE;
}

BOOL WINAPI InternetCrackUrlW(const WCHAR* lpszUrl,
                              DWORD dwUrlLength,
                              DWORD dwFlags,
                              URL_COMPONENTSW* lpUC) {
  if (!lpszUrl || !*lpszUrl || !lpUC) {
    return FALSE;
  }
  std::u16string url_convert = reinterpret_cast<const char16_t*>(lpszUrl);
  WininetUrlInfo<std::u16string> url_crack(url_convert);
  /**
   * TODO
   * 1) flag 처리
   * 2) error 처리 : return FALSE;
   * 3)
   */

  /* Window 구조체 타입
	   typedef struct {
	   DWORD dwStructSize;
	   1. scheme
	   LPWSTR lpszScheme;
	   DWORD dwSchemeLength;
	   INTERNET_SCHEME nScheme;
	   2. host name
	   LPWSTR lpszHostName;
	   DWORD dwHostNameLength;
	   3. port
	   INTERNET_PORT nPort;
	   4. user name
	   LPWSTR lpszUserName;
	   DWORD dwUserNameLength;
	   5. password
	   LPWSTR lpszPassword;
	   DWORD dwPasswordLength;
	   6. url path
	   LPWSTR lpszUrlPath;
	   DWORD dwUrlPathLength;
	   7. extra info
	   LPWSTR lpszExtraInfo;
	   DWORD dwExtraInfoLength;
	   } URL_COMPONENTSW, *LPURL_COMPONENTSW;
	   */

  lpUC->dwSchemeLength = url_crack.getProtocol().length();
  url_crack.getProtocol().copy(reinterpret_cast<char16_t*>(lpUC->lpszScheme),
                               lpUC->dwSchemeLength);
  lpUC->dwHostNameLength = url_crack.getHostName().length();
  url_crack.getHostName().copy(reinterpret_cast<char16_t*>(lpUC->lpszHostName),
                               lpUC->dwHostNameLength);
  lpUC->dwUserNameLength = url_crack.getUserName().length();
  url_crack.getUserName().copy(reinterpret_cast<char16_t*>(lpUC->lpszUserName),
                               lpUC->dwUserNameLength);
  lpUC->dwPasswordLength = url_crack.getPassword().length();
  url_crack.getPassword().copy(reinterpret_cast<char16_t*>(lpUC->lpszPassword),
                               lpUC->dwPasswordLength);
  lpUC->dwUrlPathLength = url_crack.getUrlPath().length();
  url_crack.getUrlPath().copy(reinterpret_cast<char16_t*>(lpUC->lpszUrlPath),
                              lpUC->dwUrlPathLength);
  lpUC->dwExtraInfoLength = url_crack.getExtraInfo().length();
  url_crack.getExtraInfo().copy(
      reinterpret_cast<char16_t*>(lpUC->lpszExtraInfo),
      lpUC->dwExtraInfoLength);
  /* TODO : SCHEME 및 이를 기반한 PORT 설정 */
  return TRUE;
}

BOOL WINAPI InternetGetConnectedState(LPDWORD lpdwStatus, DWORD dwReserved) {
  /**
	 * TOS network를 이용
	 */
  FILE* fp = nullptr;
  char buf[64] = {0}, dev[64] = {0}, *ptr = nullptr;
  const char* cui_command("/system/bin/network_cui -d");
  fp = popen(cui_command, "r");
  if (fp == nullptr) {
    fprintf(stderr, "popen failed\n");
    return false;
  }
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    if ((ptr = strstr(buf, "connected_device :")) != nullptr) {
      sscanf(ptr, "connected_device : %s", dev);
      if (strncmp(dev, "NONE", 4) == 0) {
        if (fp)
          pclose(fp);
        return false;
      }
      *lpdwStatus = INTERNET_CONNECTION_LAN;
      break;
    }
  }
  if (fp)
    pclose(fp);
  return true;
}

/* <PART4> Cookies related functions */

BOOL WINAPI InternetSetCookieW(const WCHAR* url,
                               const WCHAR* name,
                               const WCHAR* data) {
  std::string url_8 = UTF16ToUTF8(reinterpret_cast<const char16_t*>(url));
  std::string data_8 = UTF16ToUTF8(reinterpret_cast<const char16_t*>(data));
  WininetUrlInfo<std::string> url_info(url_8);

  /* STEP1 : set valid domain / host name */
  std::string domain = WininetUtil::GetDomainFromData(data_8);
  if (domain.empty()) {
    domain = url_info.getHostName();
  }
  std::string path = WininetUtil::GetPathFromData(data_8);
  if (path.empty()) {
    path = url_info.getUrlPath();
  }

  /* STEP2 : set cookie */
  WininetCookieContainerList* container_list =
      WininetCookieContainerList::GetInstance();
  if (container_list) {
    WininetCookieContainer* container =
        container_list->createCookieContainer(domain);
    if (container) {
      WininetCookie* cookie = container->createCookieWithPath(path);
      if (cookie) {
        cookie->setCookie(data_8);
      } else {
        /* fail to create new cookie */
        return false;
      }
    } else {
      /* fail to create new container */
      return false;
    }
  } else {
    return false;
  }
  return true;
}

BOOL WINAPI InternetGetCookieExW(LPCWSTR lpszUrl,
                                 LPCWSTR lpszCookieName,
                                 LPWSTR lpCookieData,
                                 LPDWORD lpdwSize,
                                 DWORD flags,
                                 void* reserved) {
  std::string url_8 = UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszUrl));
  WininetUrlInfo<std::string> url_info(url_8);
  std::string domain(url_info.getHostName()), path(url_info.getUrlPath()),
      path_tmp(path), cookie_list;
  WininetCookieContainer* container = nullptr;
  WininetCookie* cookie_path = nullptr;

  /* STEP1 : parsing cookies from domain(path) and its parent */
  WininetCookieContainerList* container_list =
      WininetCookieContainerList::GetInstance();
  if (container_list) {
    while (!domain.empty()) {
      container = container_list->findCookieContainer(domain);
      if (container) {
        while (!path_tmp.empty()) {
          cookie_path = container->findCookieWithPath(path_tmp);
          if (cookie_path) {
            std::string cookies = cookie_path->getCookies();
            cookie_list.append(cookies);
          }
          path_tmp = WininetUtil::GetParentPath(path_tmp);
        }
      }
      domain = WininetUtil::GetParentDomain(domain);
      path_tmp = path;
    }
  }

  /* STEP2 : check length/data of cookies and copy */
  std::u16string cookie_list_16 = UTF8ToUTF16(cookie_list);
  size_t cookie_length = cookie_list_16.length();
  if (!cookie_length) {
    /* TODO : SetLastError -> ERROR_NO_MORE_ITEMS */
    return false;
  }
  if (*lpdwSize < cookie_length) {
    /* TODO : setLastError -> ERROR_INSUFFICIENT_BUFFER */
    *lpdwSize = cookie_length;
    return false;
  }
  *lpdwSize = cookie_length;
  cookie_list_16.copy(reinterpret_cast<char16_t*>(lpCookieData), cookie_length);
  return true;
}

BOOL WINAPI InternetGetLastResponseInfoW(LPDWORD lpdwError,
                                         LPWSTR lpszBuffer,
                                         LPDWORD lpdwBufferLength) {
  return true;
}

/* <PART5> HINTERNET handle */

HINTERNET WINAPI InternetOpenW(LPCWSTR lpszAgent,
                               DWORD dwAccessType,
                               LPCWSTR lpszProxy,
                               LPCWSTR lpszProxyBypass,
                               DWORD dwFlags) {
  /* TODO : CreateHandle encapsulation, */
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetRootSession* root_session =
      handle_table->createRootObject(WininetHandleType::WININET_INTERNET);
  if (root_session) {
    struct WininetRootInfo root_info;
    struct WininetProxyInfo proxy_info;

    root_info.access_type = dwAccessType;
    root_info.agent = UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszAgent));
    proxy_info.proxy_server =
        UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszProxy));
    proxy_info.proxy_bypass =
        UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszProxyBypass));

    /* session 등록 */
    root_session->setRootInfo(root_info);
    root_session->setProxyInfo(proxy_info);
    root_session->setRootFlag(dwFlags);
    return root_session->getHandle();
  } else {
    return HINTERNET(-1);
  }
}

#if 0
HINTERNET WINAPI InternetOpenA(LPCSTR lpszAgent,
                               DWORD dwAccessType,
                               LPCSTR lpszProxy,
                               LPCSTR lpszProxyBypass,
                               DWORD dwFlags) {
  /* TODO : CreateHandle encapsulation, */
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetRootSession* root_session =
      handle_table->createRootObject(WininetHandleType::WININET_INTERNET);
  if (root_session) {
    struct WininetRootInfo root_info;
    struct WininetProxyInfo proxy_info;

    root_info.access_type = dwAccessType;
    root_info.agent = AnsiToUTF8(lpszAgent);
    proxy_info.proxy_server = AnsiToUTF8(lpszProxy);
    proxy_info.proxy_bypass = AnsiToUTF8(lpszProxyBypass);

    /* session 등록 */
    root_session->setRootInfo(root_info);
    root_session->setProxyInfo(proxy_info);
    root_session->setRootFlag(dwFlags);
    return root_session->getHandle();
  } else {
    return HINTERNET(-1);
  }
}
#endif

HINTERNET WINAPI InternetConnectW(HINTERNET hInternet,
                                  LPCWSTR lpszServerName,
                                  INTERNET_PORT nServerPort,
                                  LPCWSTR lpszUserName,
                                  LPCWSTR lpszPassword,
                                  DWORD dwService,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext) {
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetBase* root_session = handle_table->getObjectFromHandle(hInternet);

  struct WininetConnectInfo connect_info;
  connect_info.server_port = nServerPort;
  connect_info.server_name =
      UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszServerName));
  connect_info.user_name =
      UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszUserName));
  connect_info.password =
      UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszPassword));
  if (root_session == nullptr) {
    return NULL;
  }
  if (dwService == INTERNET_SERVICE_FTP) {
    WininetConnectFtp* connect_ftp_session =
        handle_table->createObject<class WininetConnectFtp>(root_session);
    if (connect_ftp_session) {
      connect_ftp_session->setConnectInfo(connect_info);
      connect_ftp_session->setConnectFlags(dwFlags);
      return connect_ftp_session->getHandle();
    } else {
      return NULL;
    }
  } else if (dwService == INTERNET_SERVICE_HTTP) {
    WininetConnectHttp* connect_http_session =
        handle_table->createObject<class WininetConnectHttp>(root_session);
    if (connect_http_session) {
      connect_http_session->setConnectInfo(connect_info);
      return connect_http_session->getHandle();
    } else {
      return NULL;
    }
  } else {
    return NULL; /* FIXME : Invalid handle */
  }
}

#if 0
HINTERNET WINAPI InternetConnectA(HINTERNET hInternet,
                                  LPCSTR lpszServerName,
                                  INTERNET_PORT nServerPort,
                                  LPCSTR lpszUserName,
                                  LPCSTR lpszPassword,
                                  DWORD dwService,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext) {
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetBase* root_session = handle_table->getObjectFromHandle(hInternet);

  struct WininetConnectInfo connect_info;
  connect_info.server_port = nServerPort;
  connect_info.server_name = AnsiToUTF8(lpszServerName);
  connect_info.user_name = AnsiToUTF8(lpszUserName);
  connect_info.password = AnsiToUTF8(lpszPassword);
  if (root_session == nullptr) {
    return NULL;
  }
  if (dwService == INTERNET_SERVICE_FTP) {
    WininetConnectFtp* connect_ftp_session =
        handle_table->createObject<class WininetConnectFtp>(root_session);
    if (connect_ftp_session) {
      connect_ftp_session->setConnectInfo(connect_info);
      connect_ftp_session->setConnectFlags(dwFlags);
      return connect_ftp_session->getHandle();
    } else {
      return NULL;
    }
  } else if (dwService == INTERNET_SERVICE_HTTP) {
    WininetConnectHttp* connect_http_session =
        handle_table->createObject<class WininetConnectHttp>(root_session);
    if (connect_http_session) {
      connect_http_session->setConnectInfo(connect_info);
      return connect_http_session->getHandle();
    } else {
      return NULL;
    }
  } else {
    return NULL; /* FIXME : Invalid handle */
  }
}
#endif

HINTERNET WINAPI InternetOpenUrlW(HINTERNET hInternet,
                                  LPCWSTR lpszUrl,
                                  LPCWSTR lpszHeaders,
                                  DWORD dwHeadersLength,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext) {
  if (!lpszUrl) {
    return NULL;
  }
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();

  WininetBase* root_session = handle_table->getObjectFromHandle(hInternet);
  if (root_session == nullptr) {
    /* error에 대해 삽입할 것 */
    return NULL;
  }
  std::string url_convert =
      UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszUrl));
  WininetUrlInfo<std::string> url_crack(url_convert);

  /* parsing connect info from URL */
  struct WininetConnectInfo connect_info;
  connect_info.server_name = url_crack.getHostName();
  connect_info.server_port = std::stoi(url_crack.getPort());
  connect_info.user_name = url_crack.getUserName();
  connect_info.password = url_crack.getPassword();

  /* TODO : openUrl로 열었을 경우 additional job */
  std::string protocol = url_crack.getProtocol();
  if (protocol.compare("ftp") == 0) {
    fprintf(stderr, "FTP Handle is not implemented");
    return NULL; /* invalid handle */
  } else if (protocol.compare("http") == 0) {
    WininetConnectHttp* connect_session =
        handle_table->createObject<class WininetConnectHttp>(root_session);
    connect_session->setConnectInfo(connect_info);
    return connect_session->getHandle();
  } else {
    fprintf(stderr, "Invalid protocol\n");
    return NULL;
  }
}

#if 0
HINTERNET WINAPI InternetOpenUrlA(HINTERNET hInternet,
                                  LPCSTR lpszUrl,
                                  LPCSTR lpszHeaders,
                                  DWORD dwHeadersLength,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext) {
  if (!lpszUrl) {
    return NULL;
  }
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();

  WininetBase* root_session = handle_table->getObjectFromHandle(hInternet);
  if (root_session == nullptr) {
    /* error에 대해 삽입할 것 */
    return NULL;
  }
  std::string url_convert = AnsiToUTF8(lpszUrl);
  WininetUrlInfo<std::string> url_crack(url_convert);

  /* parsing connect info from URL */
  struct WininetConnectInfo connect_info;
  connect_info.server_name = url_crack.getHostName();
  connect_info.server_port = std::stoi(url_crack.getPort());
  connect_info.user_name = url_crack.getUserName();
  connect_info.password = url_crack.getPassword();

  /* TODO : openUrl로 열었을 경우 additional job
   * lpszHeaders
   * */
  std::string protocol = url_crack.getProtocol();
  if (protocol.compare("ftp") == 0) {
    fprintf(stderr, "FTP Handle is not implemented");
    return NULL; /* invalid handle */
  } else if (protocol.compare("http") == 0) {
    WininetConnectHttp* connect_session =
        handle_table->createObject<class WininetConnectHttp>(root_session);
    connect_session->setConnectInfo(connect_info);
    return connect_session->getHandle();
  } else {
    fprintf(stderr, "Invalid protocol\n");
    return NULL;
  }
}
#endif

HINTERNET WINAPI HttpOpenRequestW(HINTERNET hHttpSession,
                                  LPCWSTR lpszVerb,
                                  LPCWSTR lpszObjectName,
                                  LPCWSTR lpszVersion,
                                  LPCWSTR lpszReferrer,
                                  LPCWSTR* lpszAcceptTypes,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext) {
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetBase* http_session = handle_table->getObjectFromHandle(hHttpSession);
  WininetHttpRequest* http_request =
      handle_table->createObject<class WininetHttpRequest>(http_session);
  if (http_request) {
    struct HttpRequestInfo request_info;
    request_info.path =
        UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszObjectName));
    request_info.verb =
        UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszVerb));
    request_info.version =
        UTF16ToUTF8(reinterpret_cast<const char16_t*>(lpszVersion));
    http_request->setHttpRequestInfo(request_info);
    http_request->setHttpFlags(dwFlags);
  } else {
    return NULL;
  }
  /**
   * TODO
   * - AccessType에 대한 정의
   * - Referrer 에 대한 처리
   * - callback 함수 처리
   * - lock 처리
   * - security flags
   */
  return http_request->getHandle();
}

BOOL WINAPI InternetCloseHandle(HINTERNET hInternet) {
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetBase* object = handle_table->getObjectFromHandle(hInternet);
  if (object) {
    if (!handle_table->closeObject(hInternet))
      return false;
    else
      return false;
  }
  return true;
}

/* <PART6> Set/Query Option in HINTERNET HANDLE */

BOOL WINAPI InternetQueryOptionW(HINTERNET hInternet,
                                 DWORD dwOption,
                                 LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength) {
  /* setOption으로 등록된 option 조회 */
  return false;
}

/* SendRequest 이후에 read 함수를 통해 데이터 읽음 */
BOOL WINAPI InternetSetOptionW(HINTERNET hInternet,
                               DWORD dwOption,
                               LPVOID lpBuffer,
                               DWORD dwBufferLength) {
  return false;
}

INTERNET_STATUS_CALLBACK WINAPI
InternetSetStatusCallbackW(HINTERNET hInternet,
                           INTERNET_STATUS_CALLBACK lpfnIntCB) {
  /* callback : root session 내 등록 */
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetRootSession* root_session = static_cast<WininetRootSession*>(
      handle_table->getObjectFromHandle(hInternet));
  if (root_session) {
    INTERNET_STATUS_CALLBACK callback = root_session->getCallback();
    root_session->setCallback(lpfnIntCB);
    return callback; /* return previous callback */
  } else {
    return NULL; /* FIXME : 제대로 동작하지 않았을 때 */
  }
}

/* <PART7> Send/Recv Data Function */

BOOL WINAPI InternetQueryDataAvailable(HINTERNET hFile,
                                       LPDWORD lpdwNumberOfBytesAvailable,
                                       DWORD dwFlags,
                                       DWORD_PTR dwContext) {
  WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
  WininetHttpRequest* http_session = static_cast<WininetHttpRequest*>(
      handle_table->getObjectFromHandle(hFile));
  if (http_session) {
    int available_data = http_session->queryResponseDataLength();
    if (available_data == -1) {
      /* there is not available data */
      return false;
    } else {
      *lpdwNumberOfBytesAvailable = available_data;
      return true;
    }
  } else {
    /* invalid handle */
    return false;
  }
}

BOOL WINAPI InternetReadFile(HINTERNET hFile,
                             LPVOID lpBuffer,
                             DWORD dwNumOfBytesToRead,
                             LPDWORD pdwNumOfBytesRead) {
	/* FTP / HTTP 둘다 사용할 수 있음 */
	WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
	WininetHttpRequest* http_session = static_cast<WininetHttpRequest*>(
			handle_table->getObjectFromHandle(hFile));
	if (http_session) {
		int read_data = http_session->readData(lpBuffer, dwNumOfBytesToRead);
		if (read_data == -1) {
			/* there is not available data */
			return false;
		} else {
			*pdwNumOfBytesRead = read_data;
			return true;
		}
	} else {
		/* invalid handle */
		return false;
	}
}

BOOL WINAPI InternetWriteFile(HINTERNET hFile,
                              LPCVOID lpBuffer,
                              DWORD dwNumOfBytesToWrite,
                              LPDWORD lpdwNumOfBytesWritten) {
  /* Read 대신에 서버로 직접 파일을write 하고자 할 때 */
  return true;
}

/* FTP function */
BOOL WINAPI FtpPutFileW(HINTERNET hConnect,
                        LPCWSTR lpszLocalFile,
                        LPCWSTR lpszNewRemoteFile,
                        DWORD dwFlags,
                        DWORD_PTR dwContext) {
  return true;
}

BOOL WINAPI FtpSetCurrentDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory) {
  return true;
}

} /* namespace winapi */
} /* namespace toc */
