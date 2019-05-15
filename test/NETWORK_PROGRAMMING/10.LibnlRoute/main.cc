#include "netlink.h"

int main() {

	std::string ifname = "tosbr_enp2s0";
	std::string address = "192.168.8.182/24";
	std::string netmask = "255.255.255.0";
	std::string gateway = "192.168.8.1";
	unsigned long priority = 10;

	//addIpv4Address(ifname, address, netmask);
//	addIpv4Route(ifname, gateway, priority);
	delIpv4Route(ifname, gateway);
	return 0;
}
