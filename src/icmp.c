#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include "ping_config.h"

static unsigned int icmp_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static struct nf_hook_ops icmp_ops;

int init_icmp(void) {
    return nf_register_net_hook(&init_net, &my_nfho);
}

void deinit_icmp(void)
{
    nf_unregister_net_hook(&init_net, &my_nfho);
}