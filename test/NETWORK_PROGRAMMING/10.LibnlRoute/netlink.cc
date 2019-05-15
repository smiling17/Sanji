#include <netlink/cli/link.h>
#include <netlink/cli/utils.h>
#include <netlink/cli/route.h>

#include <linux/if.h>

#include <iostream>
#include <string>

#include "netlink.h"

int delIpv4Route(const std::string& ifname, const std::string& address) {
	/* cli 기반으로 한번 작성해보자 */
	struct nl_sock *sock;
	struct rtnl_route *route;
	int err = 1;
	int ival = -1;
	struct nl_cache *link_cache, *route_cache;
	const char *table = "main";
	const char *scope = "global";
	const char *protocol = "boot";
	const char* type = "unicast";
	const char *router = "192.168.8.1/24"; /* 의미없다 */
	std::cout << "Start ... " << std::endl;
	sock = nl_socket_alloc();
	if (!sock) {
	std::cout << "Error Point 1-1 ... " << std::endl;
	}
	if (nl_connect(sock, NETLINK_ROUTE) < 0) {
	std::cout << "Error Point 1-2 ... " << std::endl;
	}
	rtnl_link_alloc_cache(sock, AF_UNSPEC, &link_cache);
	rtnl_route_alloc_cache(sock, AF_UNSPEC, 0, &route_cache);
	route = rtnl_route_alloc();
	struct rtnl_nexthop *nh;
	if (!(nh = rtnl_route_nh_alloc())) {
	std::cout << "Error Point 1-3 ... " << std::endl;
	}

	/* device / address */
	if (!(ival = rtnl_link_name2i(link_cache, ifname.c_str()))) {
	std::cout << "Error Point 1-4 ... " << std::endl;
	}
	rtnl_route_nh_set_ifindex(nh, ival);
	struct nl_addr *addr;
	nl_addr_parse(address.c_str(), rtnl_route_get_family(route), &addr);
	rtnl_route_nh_set_gateway(nh, addr);
	nl_addr_put(addr);
	//rtnl_route_nh_set_weight(nh, 100);
	/* destination : all/default/any */
	nl_addr_parse("default", rtnl_route_get_family(route), &addr);
	rtnl_route_set_dst(route, addr);
	nl_addr_put(addr);

	rtnl_route_add_nexthop(route, nh);

#if 0
	/* destination */
	nl_addr_parse(router, rtnl_route_get_family(route), &addr);
	if (rtnl_route_set_dst(route, addr) < 0) {
	}
	nl_addr_put(addr);
#endif

	/* priority */

	/* scope */
	if ((ival = rtnl_str2scope(scope)) < 0) {
	std::cout << "Error Point 2 ... " << std::endl;
	}
	rtnl_route_set_scope(route, ival);

	/* table */
	int tablei;
	if ((tablei = rtnl_route_str2table(table)) < 0) {
	std::cout << "Error Point 3 ... " << std::endl;
	}
	rtnl_route_set_table(route, tablei);

	/* protocol */
	int proto;
	if ((proto = rtnl_route_str2proto(protocol)) < 0) {
	std::cout << "Error Point 4 ... " << std::endl;
	}
	rtnl_route_set_protocol(route, proto);

	/* type : unicast */
	if ((ival = nl_str2rtntype(type)) < 0) {
	std::cout << "Error Point 5 ... " << std::endl;
	}
	if ((ival = rtnl_route_set_type(route, ival)) < 0) {
	std::cout << "Error Point 6 ... " << std::endl;
	}

	/* finish */
	if ((err = rtnl_route_delete(sock, route, NLM_F_CREATE|NLM_F_EXCL)) < 0) {
		std::cout << "Unable to add route: " << nl_geterror(err) << std::endl;
	}
	/* TODO : alloc() 에 대한 memory leak */
//	nl_socket_free(sock);
	std::cout << "End... " << std::endl;
	struct nl_dump_params dp = {
		.dp_type = NL_DUMP_LINE,
		.dp_fd = stdout,
	};
	nl_object_dump(OBJ_CAST(route), &dp);
	return 0;
}

int addIpv4Route(const std::string& ifname, const std::string& address, unsigned long priority) {
	/* cli 기반으로 한번 작성해보자 */
	struct nl_sock *sock;
	struct rtnl_route *route;
	int err = 1;
	int ival = -1;
	struct nl_cache *link_cache, *route_cache;
	const char *table = "main";
	const char *scope = "global";
	const char *protocol = "boot";
	const char* type = "unicast";
	const char *router = "192.168.8.1/24"; /* 의미없다 */
	std::cout << "Start ... " << std::endl;
	sock = nl_socket_alloc();
	if (!sock) {
	std::cout << "Error Point 1-1 ... " << std::endl;
	}
	if (nl_connect(sock, NETLINK_ROUTE) < 0) {
	std::cout << "Error Point 1-2 ... " << std::endl;
	}
	rtnl_link_alloc_cache(sock, AF_UNSPEC, &link_cache);
	rtnl_route_alloc_cache(sock, AF_UNSPEC, 0, &route_cache);
	route = rtnl_route_alloc();
	struct rtnl_nexthop *nh;
	if (!(nh = rtnl_route_nh_alloc())) {
	std::cout << "Error Point 1-3 ... " << std::endl;
	}

	/* device / address */
	if (!(ival = rtnl_link_name2i(link_cache, ifname.c_str()))) {
	std::cout << "Error Point 1-4 ... " << std::endl;
	}
	rtnl_route_nh_set_ifindex(nh, ival);
	struct nl_addr *addr;
	nl_addr_parse(address.c_str(), rtnl_route_get_family(route), &addr);
	rtnl_route_nh_set_gateway(nh, addr);
	nl_addr_put(addr);
	//rtnl_route_nh_set_weight(nh, 100);
	/* destination : all/default/any */
	nl_addr_parse("default", rtnl_route_get_family(route), &addr);
	rtnl_route_set_dst(route, addr);
	nl_addr_put(addr);

	rtnl_route_add_nexthop(route, nh);

#if 0
	/* destination */
	nl_addr_parse(router, rtnl_route_get_family(route), &addr);
	if (rtnl_route_set_dst(route, addr) < 0) {
	}
	nl_addr_put(addr);
#endif

	/* priority */
	rtnl_route_set_priority(route, priority);

	/* scope */
	if ((ival = rtnl_str2scope(scope)) < 0) {
	std::cout << "Error Point 2 ... " << std::endl;
	}
	rtnl_route_set_scope(route, ival);

	/* table */
	int tablei;
	if ((tablei = rtnl_route_str2table(table)) < 0) {
	std::cout << "Error Point 3 ... " << std::endl;
	}
	rtnl_route_set_table(route, tablei);

	/* protocol */
	int proto;
	if ((proto = rtnl_route_str2proto(protocol)) < 0) {
	std::cout << "Error Point 4 ... " << std::endl;
	}
	rtnl_route_set_protocol(route, proto);

	/* type : unicast */
	if ((ival = nl_str2rtntype(type)) < 0) {
	std::cout << "Error Point 5 ... " << std::endl;
	}
	if ((ival = rtnl_route_set_type(route, ival)) < 0) {
	std::cout << "Error Point 6 ... " << std::endl;
	}

	/* finish */
	if ((err = rtnl_route_add(sock, route, NLM_F_CREATE|NLM_F_EXCL)) < 0) {
		std::cout << "Unable to add route: " << nl_geterror(err) << std::endl;
	}
	/* TODO : alloc() 에 대한 memory leak */
//	nl_socket_free(sock);
	std::cout << "End... " << std::endl;
	struct nl_dump_params dp = {
		.dp_type = NL_DUMP_LINE,
		.dp_fd = stdout,
	};
	nl_object_dump(OBJ_CAST(route), &dp);
	return 0;
}

int changeLinkState(const std::string& ifname, bool up) {
	const char* name = "tosbr_enp2s0";
	struct nl_sock *sk;
	struct rtnl_link *change, *origin;
	struct nl_cache *cache = nullptr;

	std::cout << "Start ... " << std::endl;
	sk = nl_socket_alloc();
	nl_connect(sk, NETLINK_ROUTE);
	rtnl_link_alloc_cache(sk, AF_UNSPEC, &cache);
	origin = rtnl_link_get_by_name(cache, ifname.c_str());
	change = rtnl_link_alloc();
	
	if (change == nullptr) {
		std::cout << "Fail ... " << std::endl;
		return -1;
	}
	rtnl_link_set_name(change, name);
	if (up) {
		rtnl_link_set_flags(change, IFF_UP);
	}
	else {
		rtnl_link_unset_flags(change, IFF_UP);
	}
	rtnl_link_add(sk, change, 0);
	/**
	 * FIXME : 사용상은 change로 해야 되는데 change 안먹힘..
	 * 일단 rtnl_link_add 를 하면 동작하는것 확인
	 */
	//rtnl_link_change(sk, origin, change, 0);

	/* clean up */
	rtnl_link_put(change);
	nl_socket_free(sk);
	std::cout << "Finish... " << std::endl;

}
int addIpv4Address(const std::string& ifname, const std::string& ipv4addr, const std::string netmask) {
	struct nl_sock *sock;
	struct rtnl_addr *addr;
	struct nl_cache *link_cache;
	sock = nl_socket_alloc();
	nl_connect(sock, NETLINK_ROUTE);
	rtnl_link_alloc_cache(sock, AF_UNSPEC, &link_cache);
	addr = rtnl_addr_alloc();

	/* setting device */
	int ival;
	if (!(ival = rtnl_link_name2i(link_cache, ifname.c_str()))) {
	}
	rtnl_addr_set_ifindex(addr, ival);

	/* setting address */
	int family = rtnl_addr_get_family(addr);
	struct nl_addr *a;
	if (nl_addr_parse(ipv4addr.c_str(), family, &a) < 0) {
	}
	if (rtnl_addr_set_local(addr, a) < 0 ) {
	}
	nl_addr_put(a);

	int nlflags = NLM_F_CREATE;

	/* request */
	if (rtnl_addr_add(sock, addr, nlflags) < 0) {
	}

	return 0;
}
