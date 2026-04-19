#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include "ping_config.h"

/* Swap mac and ip addr in eth packet for L2|l3 in OSI*/
static void swap_addresses(struct sk_buff *skb) {
    struct ethhdr *eth = eth_hdr(skb);
    struct iphdr *iph = ip_hdr(skb);
    __be32 tmp_ip;
    unsigned char tmp_mac[ETH_ALEN];

    ether_addr_copy(tmp_mac, eth->h_source);
    ether_addr_copy(eth->h_source, eth->h_dest);
    ether_addr_copy(eth->h_dest, tmp_mac);

    tmp_ip = iph->saddr;
    iph->saddr = iph->daddr;
    iph->daddr = tmp_ip;
}
static void update_checksums(struct sk_buff *skb, struct iphdr *iph, struct icmphdr *icmph);

static unsigned int icmp_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct icmphdr *icmph;

    iph = ip_hdr(skb);
    if (!iph || iph->protocol != IPPROTO_ICMP || iph->daddr != ip_addr) {
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