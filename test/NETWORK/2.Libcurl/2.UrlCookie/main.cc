#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <string>
#include <vector>

#include <curl/curl.h>

static void print_cookie(CURL *curl) {
	CURLcode res;
	struct curl_slist *cookies;
	struct curl_slist *nc;
	int i;

	fprintf(stdout, "===== Coockies Test start =====\n");
	res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_getinfo_failed : %s\n",
				curl_easy_strerror(res));
		exit(1);
	}
	nc = cookies;
	i = 1;
	while (nc) {
		fprintf(stdout, "[%d] : %s\n", i, nc->data);
		nc = nc->next;
		i++;
	}
	if (i == 1) {
		fprintf(stdout, "(none)\n");
	}
	fprintf(stdout, "===== Coockies Test End =====\n");
	curl_slist_free_all(cookies);
}

int main() {
	CURL *curl;
	CURLcode res;
	char* url = "https://www.google.com/";
	char* domain = ".google.com";
	bool test_cookie = true;

	curl = curl_easy_init();
	if (curl) {
		char nline[256];

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* start cookie engine */
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "Curl perform #1 failed : %s\n", curl_easy_strerror(res));
			return 1;
		}
		print_cookie(curl);

		fprintf(stdout, "Erasing curl's knowledge of cookies!\n");
		curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");

		print_cookie(curl);

		if (test_cookie) {/* set cookie : NETSCAPE format cookie */
			snprintf(nline, sizeof(nline), "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
					domain,"TRUE","/","FALSE", (unsigned long)time(NULL) + 31337UL,
					"PREF", "hello example, I'm sangjun lee");
			res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
			if (res != CURLE_OK) {
				fprintf(stderr, "CURL #2 curl_easy_setopt failed : %s\n", curl_easy_strerror(res));
				return 1;
			}
			/* HTTP-header style cookie */
			char domain_input[256];
			snprintf(domain_input, sizeof(domain_input), "domain=%s", domain);
			snprintf(nline, sizeof(nline),
					"Set-Cookie: OLD_PREF=3d141414bf4209321; "
					"expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; %s", 
					domain_input);
			res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
			if (res != CURLE_OK) {
				fprintf(stderr, "CURL #3 curl_easy_setopt failed : %s\n",
						curl_easy_strerror(res));
				return 1;
			}

			print_cookie(curl);
//			curl_easy_setopt(curl, CURLOPT_COOKIELIST, FLUSH);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				fprintf(stderr, "CURL #4 perform failed : %s\n", curl_easy_strerror(res));
				return 1;
			}
		}
		curl_easy_cleanup(curl);
	}
	else {
		fprintf(stderr, "CURL #0 init failed\n");
		return 1;
	}

}
