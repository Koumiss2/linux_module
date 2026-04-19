#include <linux/module.h>
#include "ping_config.h"

__be32 ip_addr = 0;

static int __init hooks_init(void) {
    ip_addr = in_aton(IP_ADDR_DEFAULT);

    if (proc_ip_init()) {
        return 1;
    }
    return 0;
}
static void __exit hooks_deinit(void){
    proc_ip_deinit();
}

module_init(hooks_init);
module_exit(hooks_deinit);
MODULE_LICENSE("GPL");