#include <linux/icmp.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/ip.h>
#include <net/dst.h>
#include "ping_config.h"

static struct net_device *vping_dev;

static int vping_open(struct net_device *dev) {
    netif_carrier_on(dev);
    netif_start_queue(dev);
    return 0;
}

static int vping_stop(struct net_device *dev) {
    netif_stop_queue(dev);
    netif_carrier_off(dev);
    return 0;
}

static bool vping_make_echo_reply(struct sk_buff *skb, struct net_device *dev) {
    struct iphdr *iph;
    struct icmphdr *icmph;
    unsigned int ihl;
    unsigned int icmp_len;
    __be32 tmp_addr;

    if (!pskb_may_pull(skb, sizeof(struct iphdr)))
    {
        return false;
    }

    skb_reset_network_header(skb);
    iph = ip_hdr(skb);

    if (!iph || iph->version != 4 || iph->protocol != IPPROTO_ICMP)
    {
        return false;
    }

    ihl = iph->ihl * 4;
    if (ihl < sizeof(struct iphdr))
    {
        return false;
    }

    if (!pskb_may_pull(skb, ihl + sizeof(struct icmphdr)))
    {
        return false;
    }

    if (skb_linearize(skb))
    {
        return false;
    }

    iph = ip_hdr(skb);
    if (iph->daddr != ip_addr) {
        return false;
    }

    icmph = (struct icmphdr *)((unsigned char *)iph + ihl);
    if (icmph->type != ICMP_ECHO)
    {
        return false;
    }

    icmp_len = ntohs(iph->tot_len) - ihl;
    if (icmp_len < sizeof(struct icmphdr))
    {
        return false;
    }

    tmp_addr = iph->saddr;
    iph->saddr = iph->daddr;
    iph->daddr = tmp_addr;
    iph->ttl = 64;
    iph->check = 0;
    iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

    icmph->type = ICMP_ECHOREPLY;
    icmph->checksum = 0;
    icmph->checksum = ip_compute_csum(icmph, icmp_len);

    skb->dev = dev;
    skb->protocol = htons(ETH_P_IP);
    skb->pkt_type = PACKET_HOST;
    skb->ip_summed = CHECKSUM_NONE;
    skb->skb_iif = dev->ifindex;
    skb_dst_drop(skb);
    nf_reset_ct(skb);
    skb_reset_network_header(skb);
    skb_set_transport_header(skb, ihl);
    skb_reset_mac_header(skb);
    skb_orphan(skb);
    memset(skb->cb, 0, sizeof(skb->cb));

    return true;
}

static netdev_tx_t vping_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    struct sk_buff *reply;

    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;

    reply = skb_copy(skb, GFP_ATOMIC);
    if (!reply) {
        dev->stats.tx_dropped++;
        dev_kfree_skb(skb);
        return NETDEV_TX_OK;
    }

    if (vping_make_echo_reply(reply, dev)) {
        dev->stats.rx_packets++;
        dev->stats.rx_bytes += reply->len;
        netif_rx(reply);
    } else {
        dev->stats.rx_dropped++;
        dev_kfree_skb(reply);
    }

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops vping_netdev_ops = {
    .ndo_open = vping_open,
    .ndo_stop = vping_stop,
    .ndo_start_xmit = vping_start_xmit,
};

static void vping_setup(struct net_device *dev) {
    dev->netdev_ops = &vping_netdev_ops;
    dev->type = ARPHRD_NONE;
    dev->hard_header_len = 0;
    dev->addr_len = 0;
    dev->flags |= IFF_NOARP | IFF_POINTOPOINT;
    dev->mtu = 1500;
    dev->min_mtu = 68;
    dev->max_mtu = 1500;
    dev->tx_queue_len = 0;
    dev->features |= NETIF_F_HW_CSUM | NETIF_F_NETNS_LOCAL;
    dev->needed_headroom = 0;
    dev->needed_tailroom = 0;
}

int init_netif(void) {
    int err;

    vping_dev = alloc_netdev(0, "vping%d", NET_NAME_UNKNOWN, vping_setup);
    if (!vping_dev)
        return -ENOMEM;

    err = register_netdev(vping_dev);
    if (err) {
        free_netdev(vping_dev);
        vping_dev = NULL;
        return err;
    }

    return 0;
}

void deinit_netif(void) {
    if (!vping_dev)
        return;

    unregister_netdev(vping_dev);
    free_netdev(vping_dev);
    vping_dev = NULL;
}
