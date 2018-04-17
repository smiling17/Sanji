#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

class HttpInfo {
	public:
		static HttpInfo* GetInstance() {
			if (instance == nullptr)
				instance = new HttpInfo();
			return instance;
		}
		void setDatasize(size_t size) { data_size = size; }
		void setDatastream(char* data) { data_stream = data; }
		void getDatasize() { fprintf(stdout, "[HttpInfo] getDatasize() : %d\n", data_size); }
		void getDatastream() {
			char* buf = nullptr;
			buf = (char*)malloc(sizeof(char)*(data_size+1));
			strncpy(buf, data_stream, data_size+1);
			fprintf(stdout, "[HttpInfo] getDataStream() : %s\n", buf);
			free(buf);
		}
		void freeDatastream() {
			/* 사이즈를 알수 없기 때문에 알수가 없다. */
			return;
			if (data_stream) {
				free(data_stream);
				data_stream = nullptr;
			}
		}
		static size_t MyCallback(void *contents, size_t size, size_t nmemb, void* userp);

	private:
		HttpInfo() {}
		~HttpInfo() {}
		static HttpInfo *instance;
	private:
		unsigned int data_size;
		char* data_stream;

};

HttpInfo* HttpInfo::instance = nullptr;

struct MemInfo {
	char *memory;
	size_t size;
};

static int call_count = 0;

size_t HttpInfo::MyCallback(void *contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	struct MemInfo *info = (struct MemInfo *)userp;
	fprintf(stdout, "============ MyCallback() start : %d ================\n", call_count);
	fprintf(stdout, "realsize : %d\n", realsize);
	fprintf(stdout, "memory : %s\n", info->memory);
	fprintf(stdout, "memsize : %d\n", info->size);
	fprintf(stdout, "contents length : %d\n", strlen((char*)contents));
	fprintf(stdout, "contents : %s\n", contents);
	fprintf(stdout, "============ MyCallback() end : %d ================\n", call_count);
	HttpInfo::GetInstance()->setDatasize(realsize);
	HttpInfo::GetInstance()->setDatastream((char*)contents);
	++call_count;
	return realsize;
}

int main() {
	CURL *curl_handle = nullptr;
	CURLcode res;
	struct MemInfo chunk;
	memset(&chunk, 0x0, sizeof(struct MemInfo));
	const char* url = "https://stackoverflow.com/questions/1970164/function-pointers-for-winapi-functions-stdcall-cdecl";

	curl_handle = curl_easy_init();

	if (curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		/* GET method */
		curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, HttpInfo::MyCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "sangjun-tmax");

		fprintf(stdout, "\n========== BEFORE PERFORM (lock을 걸자)==========\n");
		res = curl_easy_perform(curl_handle);
		fprintf(stdout, "\n========== AFTER PERFORM (lock을 걸자)==========\n");
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed : %s\n",
					curl_easy_strerror(res));
		}
		else {
//			fprintf(stdout, "========== GETINFO ==========\n");
//			curl_off_t dl = 0;
//			res = curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD, &dl);
//			if (!res) {
//				fprintf(stdout, "1. CURLINFO_SIZE_DOWNLOAD : %lld\n", dl);
//			}
//
//			dl = 0;
//			res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dl);
//			if (!res) {
//				fprintf(stdout, "2. CURLINFO_CONTENT_LENGTH_DOWNLOAD : %lld\n", dl);
//			}
//
//			long req;
//			res = curl_easy_getinfo(curl_handle, CURLINFO_REQUEST_SIZE, &req);
//			if (!res) {
//				fprintf(stdout, "3. CURLINFO_REQUEST_SIZE : %ld\n", req);
//			}
//
////			char *scheme = nullptr;
//			long http_version;
//			res = curl_easy_getinfo(curl_handle, CURLINFO_HTTP_VERSION, &http_version);
//			if (!res) {
//				fprintf(stdout, "4. CURLINFO_HTTP_VERSION : %d\n", http_version);
//			}

			fprintf(stdout, "%lu bytes retrieved\n", (long)chunk.size);
			fprintf(stdout, "retrieved data : %s\n", chunk.memory);
		}
		curl_easy_cleanup(curl_handle);
	}
	fprintf(stdout, "========== DATA PARSING ==========\n");
	HttpInfo *http_info = HttpInfo::GetInstance();
	http_info->getDatasize();
	http_info->getDatastream();
	http_info->freeDatastream();
	return 0;
}
