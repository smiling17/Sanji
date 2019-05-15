#ifndef TEST_LIBNL_NETLINK_H_
#define TEST_LIBNL_NETLINK_H_

#include <string>

int addIpv4Address(const std::string& ifname, const std::string& ipv4addr, const std::string netmask);
int changeLinkState(const std::string& ifname, bool up);
int addIpv4Route(const std::string& ifname, const std::string& address, unsigned long priority);
int delIpv4Route(const std::string& ifname, const std::string& address);

#endif
