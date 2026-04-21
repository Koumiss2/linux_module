#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/etherdevice.h>
#include "ping_config.h"
#ifdef ICMP_HOOK
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

static void update_checksums(struct sk_buff *skb, struct iphdr *iph, struct icmphdr *icmph) {
    icmph->checksum = 0;
    icmph->checksum = ip_compute_csum(icmph, skb->len - (iph->ihl * 4));
    iph->check = 0;
    iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
}

static unsigned int icmp_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph;
    struct icmphdr *icmph;

    iph = ip_hdr(skb);
    if (!iph || iph->protocol != IPPROTO_ICMP || iph->daddr != ip_addr) {
        return NF_ACCEPT;
    }

    icmph = icmp_hdr(skb);
    if (!icmph || icmph->type != ICMP_ECHO){
        return NF_ACCEPT;
    }
    if (skb_linearize(skb)) {
        return NF_DROP;
    }

    iph = ip_hdr(skb);
    icmph = icmp_hdr(skb);

    swap_addresses(skb);
    icmph->type = ICMP_ECHOREPLY;
    update_checksums(skb, iph, icmph);

    dev_queue_xmit(skb);

    return NF_STOLEN;
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
#endif