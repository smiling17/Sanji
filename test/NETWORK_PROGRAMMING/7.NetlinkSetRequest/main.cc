#include <asm/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>


int open_netlink() {
  struct sockaddr_nl addr;
  int sock = socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE);

  if (sock < 0)
    return sock;

  addr.nl_family = AF_NETLINK;
  addr.nl_pad = 0;
  addr.nl_pid = getpid();
  addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_NOTIFY;
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return -1;

  return sock;
}

void send_request(int sock) {
  struct {
    struct nlmsghdr nlhdr;
    struct ifaddrmsg addrmsg;
  } msg;
 
  memset(&msg, 0, sizeof(msg));

  msg.nlhdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  msg.nlhdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
  msg.nlhdr.nlmsg_type = RTM_GETADDR;
  msg.addrmsg.ifa_family = AF_INET;

  send(sock, &msg, msg.nlhdr.nlmsg_len, 0);
}

void receive_reply(int sock) {
  char buf[4096];
  int len;
  struct nlmsghdr *retmsg;

  len = recv(sock, buf, sizeof(buf), 0);

  for (retmsg = (struct nlmsghdr *) buf; NLMSG_OK (retmsg, len); retmsg = NLMSG_NEXT(retmsg, len)) {
    if (retmsg->nlmsg_type == NLMSG_DONE)
      return;

    if (retmsg->nlmsg_type == NLMSG_ERROR) {
      printf ("read_netlink: Message is an error\n");
      exit(1);
    }

    struct ifaddrmsg *retaddr = reinterpret_cast<struct ifaddrmsg *>
		(NLMSG_DATA(retmsg));

    struct rtattr *retrta = IFA_RTA(retaddr);
    int attlen = IFA_PAYLOAD(retmsg);
    for (; RTA_OK(retrta, attlen); retrta = RTA_NEXT(retrta, attlen)) {
      if (retrta->rta_type != IFA_ADDRESS) continue;

      /* Get interface name */
      struct ifinfomsg *ifi = reinterpret_cast<struct ifinfomsg *>
		  (NLMSG_DATA(retmsg));
      char ifname[1024];
      if_indextoname(ifi->ifi_index, ifname);

      /* Get Address */
      unsigned int *tmp = reinterpret_cast<unsigned int *>(RTA_DATA(retrta));
      char praddr[128];
      inet_ntop(AF_INET, tmp, praddr, sizeof(praddr));

      printf("Interface name: %s, address: %s\n", ifname, praddr);
    }
  }
}

/**
 * Actual request consists of:
 * 1. message header (type struct msghdr)
 * 2. netlink message header (type struct nlmsghdr)
 * 3. request family specific header (type struct ifaddrmsg / struct ifinfomsg / struct rtmsg)
 * 4. set of family specific attributes (type struct rtattr)
 */
int change_ip (int sock, const char *ip, int create) {

  struct {
    struct nlmsghdr nlhdr;
    struct ifaddrmsg addrmsg;
    char buf[4096];
  } req;

  memset(&req, 0, sizeof(req));

  // the initial length of the service request
  unsigned int msglen = sizeof(struct ifaddrmsg);

  unsigned int ip_address;
  inet_pton(AF_INET, ip, &ip_address);

  // add first attribute to req.buf
  struct rtattr *rta = (struct rtattr *) req.buf;
  rta->rta_type = IFA_ADDRESS;
  rta->rta_len = sizeof(struct rtattr) + 4;
  memcpy(((char *) rta) + sizeof (struct rtattr), &ip_address, sizeof(ip_address));
  msglen += rta->rta_len;

  // add second attribute
  rta = (struct rtattr *) (((char *) rta) + rta->rta_len);
  rta->rta_type = IFA_LOCAL;
  rta->rta_len = sizeof(struct rtattr) + 4;
  memcpy(((char *) rta) + sizeof(struct rtattr), &ip_address, sizeof(ip_address));
  msglen += rta->rta_len;

  // setup netlink header (struct nlmsghder)
  req.nlhdr.nlmsg_len = NLMSG_LENGTH(msglen);
  if (create) {
    req.nlhdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE;
    req.nlhdr.nlmsg_type = RTM_NEWADDR;
  }
  else {
    req.nlhdr.nlmsg_flags = NLM_F_REQUEST;
    req.nlhdr.nlmsg_type = RTM_DELADDR;
  }
  
  // setup the service header (struct ifaddrmsg)
  req.addrmsg.ifa_family = AF_INET;
  req.addrmsg.ifa_prefixlen = 32; // hardcoded
  req.addrmsg.ifa_flags = IFA_F_PERMANENT;
  req.addrmsg.ifa_scope = 0; // usually use 0 as scope for all requests
  req.addrmsg.ifa_index = if_nametoindex("enp2s0"); // index number of the interface this address is bound.
 
  // create the remote address to communicate.
  struct sockaddr_nl addr;
  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;

  // initialize and create the struct msghdr
  struct msghdr msg;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *) &addr;
  msg.msg_namelen = sizeof(addr);

  // place the pointer of RTNETLINK message to the struct msghdr
  struct iovec iov;
  iov.iov_base = (void *) &req.nlhdr;
  iov.iov_len = req.nlhdr.nlmsg_len;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

#if 0
  struct sockaddr_nl addr;
  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;

  // initialize and create the struct msghdr for msg
  struct msghdr msg;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *) &addr;
  msg.msg_namelen = sizeof(addr);

  // initialize and create the request msghdr
  struct nlmsghdr request_nl;
  request_nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  request_nl.nlmsg_type = RTM_NEWADDR;
  request_nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;

  struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(&request_nl);
  ifa->ifa_family = AF_INET;
  ifa->ifa_prefixlen = 16;
  ifa->ifa_flags = IFA_F_PERMANENT;
  ifa->ifa_scope = 0;
  ifa->ifa_index = 2;

  struct rtattr rta;
  rta.rta_type = RTA_DST;
  rta.rta_len = sizeof(struct rtattr) + 4;
  inet_pton(AF_INET, ip, ((char *) &rta + sizeof(struct rtattr));

  // place the pointer and size of the RTNETLINK message
  struct iovec iov;
  iov.iov_base = (void *) &request_nl;
  iov.iov_len = request_nl.nlmsg_len;

  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
#endif
#if 0
  char buf[4096];
  memset(buf, 0, 4096);

  struct nlmsghdr nl;
  nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  // note: the only difference between ip add and ip del operations is the nlmsg_type
  nl.nlmsg_type = RTM_NEWADDR;
  // we request kernel to send back ack for result checking
  nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;

  struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(&nl);
  ifa->ifa_family = AF_INET;
  ifa->ifa_prefixlen = 16;
  ifa->ifa_flags = IFA_F_PERMANENT;
  ifa->ifa_scope = 0;
  ifa->ifa_index = 2;

  // del local address
  struct rtattr *rta = (struct rtattr *) IFA_RTA(ifa);
  rta->rta_type = IFA_LOCAL;
  int ip_len = inet_pton(AF_INET, RTA_DATA(rta), ip);
  rta->rta_len = RTA_LENGTH(ip_len);
  // update nlmsghdr length
  nl.nlmsg_len = NLMSG_ALIGN(nl.nlmsg_len) + rta->rta_len;

  // del interface address
  int l = 4096 - nl.nlmsg_len;
  rta = (struct rtattr *) RTA_NEXT(rta, l);
  rta->rta_type = IFA_ADDRESS;
  ip_len = inet_pton(AF_INET, RTA_DATA(rta), ip);
  rta->rta_len = RTA_LENGTH(ip_len);
  // update nlmsghdr length
  nl.nlmsg_len += rta->rta_len;

  // prepare struct msghdr for sending
  struct iovec iov = {&nl, nl.nlmsg_len};
  struct msghdr msg = {sa, sizeof(*sa), &iov, 1, NULL, 0, 0};
#endif
  return sendmsg(sock, &msg, 0) < 0? -1 : 0;
}

int main() {
  int nls;

  nls = open_netlink();
  if (nls < 0) {
    printf("Netlink socket open error\n");
    exit(1);
  }

#if 0
  /* query info */
  printf("Sending Netlink message\n");

  send_request(nls);
  receive_reply(nls);
#endif
#if 1
  /* set info */
  printf("Setting IP address\n");
  
  change_ip(nls, "192.168.13.210", 1);
  receive_reply(nls);

#endif
  return 0;
}
