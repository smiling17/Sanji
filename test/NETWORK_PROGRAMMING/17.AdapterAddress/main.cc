#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/un.h> // struct sockaddr_un


void check_net_struct_size() {
	struct sockaddr s1;
	struct sockaddr_un s2;
	struct sockaddr_in s3;
	struct sockaddr_in6 s4;

	std::cout << "sizeof(sockaddr) : " << sizeof(s1) << std::endl;
	std::cout << "sizeof(sockaddr_un) : " << sizeof(s2) << std::endl;
	std::cout << "sizeof(sockaddr_in) : " << sizeof(s3) << std::endl;
	std::cout << "sizeof(sockaddr_in6) : " << sizeof(s4) << std::endl;

	return;
}

int main() {
	struct ifaddrs* ifa = nullptr;
	unsigned int index_dev = 2;

	if (!getifaddrs(&ifa)) {
		for (struct ifaddrs* ifp = ifa; ifp; ifp = ifp->ifa_next) {
			auto index = if_nametoindex(ifp->ifa_name);
			std::cerr << "Device Index : " << index << std::endl;
			if (index != index_dev) {
				std::cerr << "Keep Going...\n" << std::endl;
				continue;
			}

			char addr[32] = {0};
			auto family = ifp->ifa_addr->sa_family;
			std::cout << "Device Family : " << ((family == AF_INET) ? "AF_INET" :
					(family == AF_INET6) ? "AF_INET6" :
					(family == AF_PACKET) ? "AF_PACKET" :
					"UNKNOWN") << std::endl;
			inet_ntop(family, &(reinterpret_cast<struct sockaddr_in*>(ifp->ifa_addr)->sin_addr), addr, sizeof(addr));
			std::cout << "Device Address : " << addr << "\n" << std::endl;

#if 0
			// Test
			struct sockaddr_in in;
			inet_pton(AF_INET, "192.168.8.46", &in.sin_addr);
			std::cout << "Test : " << (ifp->ifa_addr->sa_data + 2) << std::endl;
#endif
		}
	}

	check_net_struct_size();

	return 0;
}
