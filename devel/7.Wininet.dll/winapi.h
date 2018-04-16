/**
 * @file    wininet.h
 * @brief   TODO brief documentation here.
 *
 * @history
 */

#ifndef _WININET_LIB_H
#define _WININET_LIB_H

#include "include/windows.h"
#include "include/wininet.h"

namespace toc {
namespace winapi {
/**
     * @class
     * @brief 함수의 validation 체크만 한다.
     */
BOOL WINAPI FtpPutFileW(HINTERNET hConnect,
                        LPCWSTR lpszLocalFile,
                        LPCWSTR lpszNewRemoteFile,
                        DWORD dwFlags,
                        DWORD_PTR dwContext);

BOOL WINAPI FtpGetFileA(HINTERNET hInternet,
                        LPCSTR lpszRemoteFile,
                        LPCSTR lpszNewFile,
                        BOOL fFailIfExists,
                        DWORD dwLocalFlagsAttribute,
                        DWORD dwInternetFlags,
                        DWORD_PTR dwContext);

BOOL WINAPI FtpSetCurrentDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory);

// HTTP
BOOL WINAPI HttpAddRequestHeadersW(HINTERNET hHttpRequest,
                                   LPCWSTR lpszHeader,
                                   DWORD dwHeaderLength,
                                   DWORD dwModifier);

BOOL WINAPI HttpAddRequestHeadersA(HINTERNET hHttpRequest,
                                   LPCSTR lpszHeader,
                                   DWORD dwHeaderLength,
                                   DWORD dwModifier);

BOOL WINAPI HttpEndRequestA(HINTERNET hRequest,
                            LPINTERNET_BUFFERSA lpBuffersOut,
                            DWORD dwFlags,
                            DWORD_PTR dwContext);

BOOL WINAPI HttpEndRequestW(HINTERNET hRequest,
                            LPINTERNET_BUFFERSW lpBuffersOut,
                            DWORD dwFlags,
                            DWORD_PTR dwContext);

HINTERNET WINAPI HttpOpenRequestA(HINTERNET hHttpSession,
                                  LPCSTR lpszVerb,
                                  LPCSTR lpszObjectName,
                                  LPCSTR lpszVersion,
                                  LPCSTR lpszReferrer,
                                  LPCSTR* lpszAcceptTypes,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

HINTERNET WINAPI HttpOpenRequestW(HINTERNET hHttpSession,
                                  LPCWSTR lpszVerb,
                                  LPCWSTR lpszObjectName,
                                  LPCWSTR lpszVersion,
                                  LPCWSTR lpszReferrer,
                                  LPCWSTR* lpszAcceptTypes,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

BOOL WINAPI HttpQueryInfoW(HINTERNET hHttpRequest,
                           DWORD dwInfoLevel,
                           LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength,
                           LPDWORD lpdwIndex);

BOOL WINAPI HttpQueryInfoA(HINTERNET hHttpRequest,
                           DWORD dwInfoLevel,
                           LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength,
                           LPDWORD lpdwIndex);

BOOL WINAPI HttpSendRequestExA(HINTERNET hRequest,
                               LPINTERNET_BUFFERSA lpBuffersIn,
                               LPINTERNET_BUFFERSA lpBuffersOut,
                               DWORD dwFlags,
                               DWORD_PTR dwContext);

BOOL WINAPI HttpSendRequestExW(HINTERNET hRequest,
                               LPINTERNET_BUFFERSW lpBuffersIn,
                               LPINTERNET_BUFFERSW lpBuffersOut,
                               DWORD dwFlags,
                               DWORD_PTR dwContext);

BOOL WINAPI HttpSendRequestW(HINTERNET hHttpRequest,
                             LPCWSTR lpszHeaders,
                             DWORD dwHeaderLength,
                             LPVOID lpOptional,
                             DWORD dwOptionalLength);

// URL cache
BOOL WINAPI DeleteUrlCacheEntryA(LPCSTR lpszUrlName);

BOOL WINAPI DeleteUrlCacheEntryW(LPCWSTR lpszUrlName);

BOOL WINAPI FindCloseUrlCache(HANDLE hEnumHandle);

INTERNETAPI HANDLE WINAPI
FindFirstUrlCacheEntryA(LPCSTR lpszUrlSearchPattern,
                        LPINTERNET_CACHE_ENTRY_INFOA lpFirstCacheEntryInfo,
                        LPDWORD lpdwFirstCacheEntryInfoBufferSize);

INTERNETAPI HANDLE WINAPI
FindFirstUrlCacheEntryW(LPCWSTR lpszUrlSearchPattern,
                        LPINTERNET_CACHE_ENTRY_INFOW lpFirstCacheEntryInfo,
                        LPDWORD lpdwFirstCacheEntryInfoBufferSize);

BOOL WINAPI
FindNextUrlCacheEntryA(HANDLE hEnumHandle,
                       LPINTERNET_CACHE_ENTRY_INFOA lpNextCacheEntryInfo,
                       LPDWORD lpdwNextCacheEntryInfoBufferSize);

BOOL WINAPI
FindNextUrlCacheEntryW(HANDLE hEnumHandle,
                       LPINTERNET_CACHE_ENTRY_INFOW lpNextCacheEntryInfo,
                       LPDWORD lpdwNextCacheEntryInfoBufferSize);

// Internet Common
DWORD WINAPI InternetAttemptConnect(DWORD dwReserved);

BOOL WINAPI InternetAutodial(DWORD dwFlags, HWND hwndParent);

DWORD WINAPI InternetSetFilePointer(HINTERNET hFile,
                                    LONG lDistanceToMove,
                                    PVOID pReserved,
                                    DWORD dwMoveContext,
                                    DWORD_PTR dwContext);

BOOL WINAPI InternetSetOptionW(HINTERNET hInternet,
                               DWORD dwOption,
                               LPVOID lpBuffer,
                               DWORD dwBufferLength);

BOOL WINAPI InternetSetOptionA(HINTERNET hInternet,
                               DWORD dwOption,
                               LPVOID lpBuffer,
                               DWORD dwBufferLength);

BOOL WINAPI InternetCanonicalizeUrlW(LPCWSTR lpszUrl,
                                     LPWSTR lpszBuffer,
                                     LPDWORD lpdwBufferLength,
                                     DWORD dwFlags);

BOOL WINAPI InternetCloseHandle(HINTERNET hInternet);

HINTERNET WINAPI InternetConnectW(HINTERNET hInternet,
                                  LPCWSTR lpszServerName,
                                  INTERNET_PORT nServerPort,
                                  LPCWSTR lpszUserName,
                                  LPCWSTR lpszPassword,
                                  DWORD dwService,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

HINTERNET WINAPI InternetConnectA(HINTERNET hInternet,
                                  LPCSTR lpszServerName,
                                  INTERNET_PORT nServerPort,
                                  LPCSTR lpszUserName,
                                  LPCSTR lpszPassword,
                                  DWORD dwService,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

BOOL WINAPI InternetCrackUrlA(const char* url,
                              DWORD url_length,
                              DWORD flags,
                              URL_COMPONENTSA* lpUC);

BOOL WINAPI InternetCrackUrlW(const WCHAR* lpszUrl,
                              DWORD dwUrlLength,
                              DWORD dwFlags,
                              URL_COMPONENTSW* lpUC);

BOOL WINAPI InternetGetConnectedState(LPDWORD lpdwStatus, DWORD dwReserved);

BOOL WINAPI InternetGetCookieExW(LPCWSTR lpszUrl,
                                 LPCWSTR lpszCookieName,
                                 LPWSTR lpCookieData,
                                 LPDWORD lpdwSize,
                                 DWORD flags,
                                 void* reserved);

BOOL WINAPI InternetGetLastResponseInfoW(LPDWORD lpdwError,
                                         LPWSTR lpszBuffer,
                                         LPDWORD lpdwBufferLength);

HINTERNET WINAPI InternetOpenW(LPCWSTR lpszAgent,
                               DWORD dwAccessType,
                               LPCWSTR lpszProxy,
                               LPCWSTR lpszProxyBypass,
                               DWORD dwFlags);

HINTERNET WINAPI InternetOpenA(LPCSTR lpszAgent,
                               DWORD dwAccessType,
                               LPCSTR lpszProxy,
                               LPCSTR lpszProxyBypass,
                               DWORD dwFlags);

HINTERNET WINAPI InternetOpenUrlW(HINTERNET hInternet,
                                  LPCWSTR lpszUrl,
                                  LPCWSTR lpszHeaders,
                                  DWORD dwHeadersLength,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

HINTERNET WINAPI InternetOpenUrlA(HINTERNET hInternet,
                                  LPCSTR lpszUrl,
                                  LPCSTR lpszHeaders,
                                  DWORD dwHeadersLength,
                                  DWORD dwFlags,
                                  DWORD_PTR dwContext);

BOOL WINAPI InternetQueryDataAvailable(HINTERNET hFile,
                                       LPDWORD lpdwNumberOfBytesAvailable,
                                       DWORD dwFlags,
                                       DWORD_PTR dwContext);

BOOL WINAPI InternetQueryOptionW(HINTERNET hInternet,
                                 DWORD dwOption,
                                 LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength);

BOOL WINAPI InternetQueryOptionA(HINTERNET hInternet,
                                 DWORD dwOption,
                                 LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength);

BOOL WINAPI InternetReadFile(HINTERNET hFile,
                             LPVOID lpBuffer,
                             DWORD dwNumOfBytesToRead,
                             LPDWORD pdwNumOfBytesRead);

BOOL WINAPI InternetSetCookieW(const WCHAR* url,
                               const WCHAR* name,
                               const WCHAR* data);

INTERNET_STATUS_CALLBACK WINAPI
InternetSetStatusCallbackA(HINTERNET hInternet,
                           INTERNET_STATUS_CALLBACK lpfnIntCB);

INTERNET_STATUS_CALLBACK WINAPI
InternetSetStatusCallbackW(HINTERNET hInternet,
                           INTERNET_STATUS_CALLBACK lpfnIntCB);

BOOL WINAPI InternetWriteFile(HINTERNET hFile,
                              LPCVOID lpBuffer,
                              DWORD dwNumOfBytesToWrite,
                              LPDWORD lpdwNumOfBytesWritten);

/*******************************************************************
 * 아래부터는 호환에 필요하지 않으나
 * 빌드에 필요한 내용들 정리
 */

BOOL WINAPI InternetFindNextFileW(HINTERNET hFind, LPVOID lpvFindData);
BOOL WINAPI InternetCrackUrlW(const WCHAR* lpszUrl,
                              DWORD dwUrlLength,
                              DWORD dwFlags,
                              URL_COMPONENTSW* lpUC);
BOOL WINAPI InternetTimeFromSystemTimeW(const SYSTEMTIME* time,
                                        DWORD format,
                                        LPWSTR string,
                                        DWORD size);
BOOL WINAPI InternetTimeToSystemTimeA(LPCSTR string,
                                      SYSTEMTIME* time,
                                      DWORD reserved);
BOOL WINAPI InternetTimeToSystemTimeW(LPCWSTR string,
                                      SYSTEMTIME* time,
                                      DWORD reserved);
BOOL WINAPI InternetCreateUrlW(LPURL_COMPONENTSW lpUrlComponents,
                               DWORD dwFlags,
                               LPWSTR lpszUrl,
                               LPDWORD lpdwUrlLength);
BOOL WINAPI InternetGetSecurityInfoByURLW(LPCWSTR lpszURL,
                                          PCCERT_CHAIN_CONTEXT* ppCertChain,
                                          DWORD* pdwSecureFlags);

/* FTP Session */

HINTERNET WINAPI FtpOpenFileW(HINTERNET hFtpSession,
                              LPCWSTR lpszFileName,
                              DWORD fdwAccess,
                              DWORD dwFlags,
                              DWORD_PTR dwContext);

BOOL WINAPI FtpGetFileW(HINTERNET hInternet,
                        LPCWSTR lpszRemoteFile,
                        LPCWSTR lpszNewFile,
                        BOOL fFailIfExists,
                        DWORD dwLocalFlagsAttribute,
                        DWORD dwInternetFlags,
                        DWORD_PTR dwContext);

} /* namespace winapi */
} /* namespace toc */

#endif /* _WININET_LIB_H */
