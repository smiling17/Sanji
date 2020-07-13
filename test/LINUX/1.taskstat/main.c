#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/taskstats.h>
#include <libnl3/netlink/netlink.h>
#include <libnl3/netlink/genl/genl.h>
#include <libnl3/netlink/genl/ctrl.h>

int callback_message(struct nl_msg *, void *);

int main(int argc, char ** argv) {
    struct nl_sock * sock;
    struct nl_msg * msg;
    int family;

    sock = nl_socket_alloc();

    // Connect to generic netlink socket on kernel side
    genl_connect(sock);

    // get the id for the TASKSTATS generic family
    family = genl_ctrl_resolve(sock, "TASKSTATS");

    // Allocate a new netlink message and inherit netlink message header. 
    msg = nlmsg_alloc();

    genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, 0, TASKSTATS_CMD_GET, TASKSTATS_VERSION);

    //error code: -7 NLE_INVAL "Invalid input data or parameter",
	nla_put_string(msg, TASKSTATS_CMD_ATTR_REGISTER_CPUMASK, "0");
	nla_put_u32(msg, TASKSTATS_CMD_ATTR_PID, 1888);

    //error code: -12 NLE_OBJ_NOTFOUND "Obj not found"
    //nla_put_string(msg, TASKSTATS_CMD_ATTR_PID, "583");

    nl_send_auto(sock, msg);

    nlmsg_free(msg);

    // specify a callback for inbound messages
    nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, callback_message, NULL);

    // gives error code -7 or -12 depending on the two nla_put_string alternatives above
    printf("recv code (0 = success): %d\n", nl_recvmsgs_default(sock));
}

int callback_message(struct nl_msg * nlmsg, void * arg) {

    struct nlmsghdr * nlhdr;
    struct nlattr * nlattrs[TASKSTATS_TYPE_MAX + 1];
    struct nlattr * nlattr;
    struct taskstats * stats;
    int rem;

    nlhdr = nlmsg_hdr(nlmsg);
    int answer;

    if ((answer = genlmsg_parse(nlhdr, 0, nlattrs, TASKSTATS_TYPE_MAX, NULL))
            < 0) {
        printf("error parsing msg\n");
    }

    if ((nlattr = nlattrs[TASKSTATS_TYPE_AGGR_TGID]) || (nlattr =
            nlattrs[TASKSTATS_TYPE_AGGR_PID]) || (nlattr =
            nlattrs[TASKSTATS_TYPE_NULL])) {
        stats = nla_data(nla_next(nla_data(nlattr), &rem));

        printf("---\n");
        printf("pid: %u\n", stats->ac_pid);
        printf("command: %s\n", stats->ac_comm);
        printf("status: %u\n", stats->ac_exitcode);
        printf("time:\n");
        printf("  start: %u\n", stats->ac_btime);
        printf("  elapsed: %llu\n", stats->ac_etime);
        printf("  user: %llu\n", stats->ac_utime);
        printf("  system: %llu\n", stats->ac_stime);
        printf("memory:\n");
        printf("  bytetime:\n");
        printf("    rss: %llu\n", stats->coremem);
        printf("    vsz: %llu\n", stats->virtmem);
        printf("  peak:\n");
        printf("    rss: %llu\n", stats->hiwater_rss);
        printf("    vsz: %llu\n", stats->hiwater_vm);
        printf("io:\n");
        printf("  bytes:\n");
        printf("    read: %llu\n", stats->read_char);
        printf("    write: %llu\n", stats->write_char);
        printf("  syscalls:\n");
        printf("    read: %llu\n", stats->read_syscalls);
        printf("    write: %llu\n", stats->write_syscalls);
    } else {
        printf("unknown attribute format received\n");
    }
    return 0;
}
