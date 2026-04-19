#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include "ping_config.h"

static unsigned int icmp_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip;

    iph = ip_hdr(skb);
    if (!iph) {
        return NF_ACCEPT;
    }

    return NF_ACCEPT;
}
static struct nf_hook_ops icmp_ops = {
    .hook     = icmp_hook,
    .hooknum  = NF_INET_PRE_ROUTING,
    .pf       = PF_INET,
    .priority = NF_IP_PRI_FIRST,  
};

int init_icmp(void) {
    return nf_register_net_hook(&init_net, &icmp_ops);
}

void deinit_icmp(void)
{
    nf_unregister_net_hook(&init_net, &icmp_ops);
}