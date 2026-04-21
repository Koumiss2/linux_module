#include <linux/module.h>
#include "ping_config.h"

__be32 ip_addr = 0;

static int __init hooks_init(void) {
    int err;

    ip_addr = in_aton(IP_ADDR_DEFAULT);

    err = init_proc_ip();
    if (err)
        return err;

    err = init_netif();
    if (err) {
        deinit_proc_ip();
        return err;
    }

    return 0;
}

static void __exit hooks_deinit(void) {
    deinit_netif();
    deinit_proc_ip();
}

module_init(hooks_init);
module_exit(hooks_deinit);
MODULE_LICENSE("GPL");
