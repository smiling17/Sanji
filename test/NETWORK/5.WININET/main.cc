#include <string>
#include <iostream>

#include "wininet_cookie.h"
#include "wininet_handle_base.h"
#include "wininet_handle_table.h"
#include "wininet_manager.h"
#include "wininet_type.h"
#include "wininet_utility.h"

int main() {
	using namespace toc::tuk2;
	fprintf(stdout, "===== MY TEST START =====\n");
	fprintf(stdout, "===== PART1 : ROOT HANDLE OPEN/SET =====\n");
	HINTERNET root_handle = NULL;
	WininetHandleTable* handle_table = WininetHandleTable::getHandleTable();
	WininetRootSession* root_session =
		handle_table->createRootObject(WininetHandleType::WININET_INTERNET);
	if (root_session) {
		struct WininetRootInfo root_info;
		root_info.async = false;
		struct WininetProxyInfo proxy_info;
		root_session->setRootInfo(root_info);
		root_session->setProxyInfo(proxy_info);
		root_handle = root_session->getHandle();
	}
	else {
		fprintf(stdout, " ROOT HANDLE FAIL\n");
		return -1;
	}
	fprintf(stdout, "===== PART2 : HTTP CONNECT HANDLE OPEN/SET =====\n");
	HINTERNET connect_handle = NULL;
	WininetBase *root_session_base = handle_table->getObjectFromHandle(root_handle);
	WininetConnectHttp *connect_session = nullptr;
	if (root_session_base) {
		connect_session = handle_table->createObject<class WininetConnectHttp>(root_session_base);
		if (connect_session) {
			struct WininetConnectInfo connect_info;
//			connect_info.server_port = 0;
			connect_info.server_name = "www.naver.com";
			connect_info.server_name = "https://stackoverflow.com/questions/1970164/function-pointers-for-winapi-functions-stdcall-cdecl";
//			connect_info.user_name = "sangjun_lee";
//			connect_info.password = "1234";
			connect_session->setConnectInfo(connect_info);
			connect_handle = connect_session->getHandle();
		}
		else {
			fprintf(stdout, " CREATE CONNECT SESSION FAIL\n");
		}
	}
	else {
		fprintf(stdout, "GET ROOT SESSION FAIL\n");
		return -1;
	}
	fprintf(stdout, "===== PART3 : HTTP REQUEST HANDLE OPEN/SET =====\n");
	HINTERNET request_handle = NULL;
	WininetHttpRequest *request_session = nullptr;
	WininetBase *connect_session_base = handle_table->getObjectFromHandle(connect_handle);
	if (connect_session_base) {
		request_session = handle_table->createObject<class WininetHttpRequest>(connect_session_base);
		if (request_session) {
			struct HttpRequestInfo request_info;
//			request_info.path = "/trunk/src";
			request_info.verb = "GET";
			request_session->setHttpRequestInfo(request_info);
			request_handle = request_session->getHandle();
		}
		else {
			fprintf(stdout, "CREATE HTTP REQUEST FAIL\n");
			return -1;
		}
	}
	else {
		fprintf(stdout, "HANDLE TABLE ERROR\n");
		return -1;
	}
	fprintf(stdout, "===== PART4 : GETTER TEST =====\n");
	fprintf(stdout, "root handle : %d\n", int(root_handle));
	fprintf(stdout, "connect handle : %d\n", int(connect_handle));
	fprintf(stdout, "request handle : %d\n", int(request_handle));
	WininetHttpRequest* test = static_cast<WininetHttpRequest*>(handle_table->getObjectFromHandle(request_handle));
	if (test) {
		WininetRootInfo* root_info_test = test->getRootInfo();
		if (root_info_test) {
			fprintf(stdout, "\n!!!!!root info enter!!!!!\n");
		}
		WininetConnectInfo* connect_info_test = test->getConnectInfo();
		if (connect_info_test) {
			fprintf(stdout, "\n!!!!!connect info enter!!!!!\n");
			fprintf(stdout, "server name : %s\n", connect_info_test->server_name.c_str());
			fprintf(stdout, "user name : %s\n", connect_info_test->user_name.c_str());
			fprintf(stdout, "password : %s\n", connect_info_test->password.c_str());
		}
		HttpRequestInfo* request_info_test = test->getHttpRequestInfo();
		if (request_info_test) {
			fprintf(stdout, "\n!!!!!request info enter!!!!!\n");
			fprintf(stdout, "path : %s\n", request_info_test->path.c_str());
			fprintf(stdout, "verb : %s\n", request_info_test->verb.c_str());
		}
	}
	else {
		fprintf(stdout, "HANDLE TEST FAIL\n");
		return -1;
	}
	fprintf(stdout, "===== PART5 : LIBCURL TEST =====\n");
	if (test->sendRequest() != 0) {
		fprintf(stdout, "SEND REQUEST FAIL\n");
		return -1;
	}
	else {
		fprintf(stdout, "SEND REQUEST SUCCESS\n");
		test->printResponseHeader();
		int length = -1;
		while ((length = test->queryResponseDataLength()) > 0) {
//			fprintf(stdout, "RESPONSE LENGTH : %d\n", length);
			void* buffer = malloc(length);
			int read_data = test->readData(buffer, length);
//			if (read_data) {
//				fprintf(stdout, "READ DATA : %d\n", read_data);
//				fprintf(stdout, "%s\n", buffer);
//			}
		}
	}
	fprintf(stdout, "\n===== MY TEST END =====\n\n");
	return 0;
}
