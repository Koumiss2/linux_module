#include <linux/module.h>
#include "icmp_proc_cfg.h"

ip_addr = in_aton(IP_ADDR_DEFAULT);

static int __init hooks_init(void);
static void __exit hooks_deinit(void);

module_init(hooks_init);
module_exit(hooks_deinit);
MODULE_LICENSE("GPL");