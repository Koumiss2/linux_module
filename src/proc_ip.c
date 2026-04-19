#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "icmp_proc_cfg.h"

/*TODO: now it only printing ip_addr in procfs for tests*/

static int proc_ip_show(struct seq_file *m, void *v) {
    seq_print(m, "IPv4: %pI4\n", &ip_addr)
}