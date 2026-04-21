#ifndef PING_CONFIG_H
#define PING_CONFIG_H

#include <linux/types.h>
#include <linux/inet.h>

#define IP_ADDR_DEFAULT "192.168.1.67" //SixSeven
#define NETIF

extern __be32 ip_addr;

int init_proc_ip(void);
void deinit_proc_ip(void);

int init_netif(void);
void deinit_netif(void);

#endif
