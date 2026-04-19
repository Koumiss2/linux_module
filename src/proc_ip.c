#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "ping_config.h"

/*TODO: now it only printing ip_addr in procfs for tests*/

static int proc_ip_show(struct seq_file *m, void *v) {
    seq_print(m, "IPv4: %pI4\n", &ip_addr);
    return 0;
}

static proc_ip_open(struct inode *inode, struct file *file) {
    .proc_open    = proc_ip_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
}

int init_proc_ip(void) {
    proc_create("proc_ip", 0644, NULL, &my_proc_ops)
}

void deinit_proc_ip(void) {
    remove_proc_entry("proc_ip", NULL);
}