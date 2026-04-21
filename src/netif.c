#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/in.h>
#include "ping_config.h"

static struct net_device *vping_dev;

static int vping_open(struct net_device *dev) {
    netif_start_queue(dev);
    return 0;
}

static int vping_stop(struct net_device *dev) {
    netif_stop_queue(dev);
    return 0;
}

static netdev_tx_t vping_start_xmit(struct sk_buff *skb, struct net_device *dev) {
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops vping_netdev_ops = {
    .ndo_open = vping_open,
    .ndo_stop = vping_stop,
    .ndo_start_xmit = vping_start_xmit,
    .ndo_set_mac_address = eth_mac_addr,
    .ndo_validate_addr = eth_validate_addr,
};

static void vping_setup(struct net_device *dev) {
    ether_setup(dev);

    dev->netdev_ops = &vping_netdev_ops;
    dev->flags |= IFF_NOARP;
    dev->features |= NETIF_F_HW_CSUM;
    dev->mtu = ETH_DATA_LEN;
    dev->min_mtu = 68;
    dev->max_mtu = ETH_DATA_LEN;
    eth_hw_addr_random(dev);
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

    pr_info("netif_name: %s\n", vping_dev->name);
    return 0;
}

void deinit_netif(void) {
    if (!vping_dev)
        return;

    unregister_netdev(vping_dev);
    free_netdev(vping_dev);
    vping_dev = NULL;
}
