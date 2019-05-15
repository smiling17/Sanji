#include <asm/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <netinet/in.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <arpa/inet.h>

#define MYPROTO NETLINK_ROUTE
#define MYMGRP RTMGRP_IPV4_ROUTE

int open_netlink();
int read_event(int sockint, int (*msg_handler)(struct sockaddr_nl *,
			struct nlmsghdr *));

/* link state로 부터 ifname, up/down 받아옴 */
static int netlink_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg);
/* msg 받았을 때의 처리 */
static int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg);

int open_netlink()
{
	int sock = socket(AF_NETLINK,SOCK_RAW,MYPROTO);
	struct sockaddr_nl addr;

	memset((void *)&addr, 0, sizeof(addr));

	if (sock<0)
		return sock;
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = getpid();
	/* monitoring 하고싶은 group 지정 */
	addr.nl_groups = RTMGRP_LINK|RTMGRP_IPV4_ROUTE|RTMGRP_IPV4_IFADDR|RTMGRP_IPV6_IFADDR;
	if (bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
		return -1;
	return sock;
}


int read_event(int sockint, int (*msg_handler)(struct sockaddr_nl *,
			struct nlmsghdr *))
{
	int status;
	int ret = 0;
	char buf[4096];
	struct iovec iov = { buf, sizeof buf };
	struct sockaddr_nl snl;
	struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0};
	struct nlmsghdr *h;

	status = recvmsg(sockint, &msg, 0);

	if(status < 0)
	{
		/* Socket
		 * non-blocking so
		 * bail out once we
		 * have read
		 * everything */
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ret;

		/* Anything
		 * else is
		 * an error
		 * */
		printf("read_netlink: Error recvmsg: %d\n", status);
		perror("read_netlink: Error: ");
		return status;
	}

	if(status == 0)
	{
		printf("read_netlink: EOF\n");
	}

	/* We need to handle more than
	 * one message per 'recvmsg' */
	for(h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); 
			h = NLMSG_NEXT (h, status))
	{
		/* Finish
		 * reading
		 * */
		if (h->nlmsg_type == NLMSG_DONE)
			return ret;

		/* Message
		 * is
		 * some
		 * kind
		 * of
		 * error
		 * */
		if (h->nlmsg_type == NLMSG_ERROR)
		{
			printf("read_netlink: Message is an error - decode TBD\n");
			return -1; // Error
		}

		/* Call
		 * message
		 * handler
		 * */
		if(msg_handler)
		{
			ret = (*msg_handler)(&snl, h);
			if(ret < 0)
			{
				printf("read_netlink: Message hander error %d\n", ret);
				return ret;
			}
		}
		else
		{
			printf("read_netlink: Error NULL message handler\n");
			return -1;
		}
	}



	return ret;
}

static int netlink_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg)
{
	int len;
	struct ifinfomsg *ifi;

	nl = nl;

	ifi = reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(msg));
	char ifname[1024];if_indextoname(ifi->ifi_index,ifname);

	printf("***** netlink_link_state: Link %s %s *****\n", 
			/*(ifi->ifi_flags &
			 * IFF_RUNNING)?"Up":"Down");*/
		ifname,(ifi->ifi_flags & IFF_UP)?"Up":"Down");
	return 0;
}

static int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg)
{
	struct ifinfomsg *ifi=reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(msg));
	struct ifaddrmsg *ifa=reinterpret_cast<struct ifaddrmsg *>(NLMSG_DATA(msg));
	struct rtmsg     *ifm=reinterpret_cast<struct rtmsg *>(NLMSG_DATA(msg));
	char ifname[1024];
	switch (msg->nlmsg_type)
	{
		case RTM_NEWROUTE:
			{
				struct rtattr *route_attr = (struct rtattr *) RTM_RTA(ifm);
				unsigned int route_attr_len = RTM_PAYLOAD(msg);

				char dest_addr[32];
				char gateway_addr[32];
				int out_index = 0;
				char out_interface[32];
				for(; RTA_OK(route_attr, route_attr_len); route_attr = RTA_NEXT(route_attr, route_attr_len)) {
					/* Get
					 * the
					 * destination
					 * address
					 * */
					if (route_attr->rta_type == RTA_DST) {
						inet_ntop(AF_INET, RTA_DATA(route_attr), dest_addr, sizeof(dest_addr));
					}
					/* Get
					 * the
					 * gateway
					 * (next
					 * hop)
					 * */
					if (route_attr->rta_type == RTA_GATEWAY) {
						inet_ntop(AF_INET, RTA_DATA(route_attr), gateway_addr, sizeof(gateway_addr));
					}
					if (route_attr->rta_type == RTA_OIF) {
						out_index = * ((int *) RTA_DATA(route_attr));
					}
				}
				printf("msg_handler: RTM_NEWROUTE : ");
				if_indextoname(out_index, out_interface);
				printf("Adding route to destination: %s and gateway: %s, interface: %d(%s)\n",
						dest_addr, gateway_addr, out_index, out_interface);
				break;
			}
		case RTM_DELROUTE:
			printf("msg_handler: RTM_DELROUTE\n");
			break;
		case RTM_NEWADDR:
			if_indextoname(ifi->ifi_index,ifname);
			printf("msg_handler: RTM_NEWADDR : %s\n",ifname);
			break;
		case RTM_DELADDR:
			if_indextoname(ifi->ifi_index,ifname);
			printf("msg_handler: RTM_DELADDR : %s\n",ifname);
			break;
		case RTM_NEWLINK:
			if_indextoname(ifa->ifa_index,ifname);
			printf("msg_handler: RTM_NEWLINK\n");
			netlink_link_state(nl, msg);
			break;
		case RTM_DELLINK:
			if_indextoname(ifa->ifa_index,ifname);
			printf("msg_handler: RTM_DELLINK : %s\n",ifname);
			break;
		default:
			printf("msg_handler: Unknown netlink nlmsg_type %d\n",
					msg->nlmsg_type);
			break;
	}
	return 0;
}
int main(int argc, char *argv[])
{
	int nls = open_netlink();
	printf("Started watching:\n");
	if (nls<0) {
		printf("Open Error!");
	}
	while (1)
		read_event(nls, msg_handler);
	return 0;
}
