#ifndef ICMP_H 
#define ICMP_H

#include <linux/types.h>

#define IP_ADDR_DEFAULT "192.168.1.21"

extern struct in_addr ip_addr;

int init_icmp(void);
void deinit_icmp(void);

int init_proc(void);
void deinit_proc(void);

#endif
