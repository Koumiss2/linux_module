#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "ping_config.h"

/*TODO: now it only printing ip_addr in procfs for tests*/

static int proc_ip_show(struct seq_file *m, void *v) {
    seq_printf(m, "IPv4: %pI4\n", &ip_addr);
    return 0;
}

static int proc_ip_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_ip_show, NULL);
}

static ssize_t proc_ip_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
    char buf[16];
    size_t len = count;

    if (len > sizeof(buf) - 1)
        len = sizeof(buf) - 1;

    if (copy_from_user(buf, ubuf, len))
        return -EFAULT;

    buf[len] = '\0';
    ip_addr = in_aton(buf);

    return count;
}

static const struct proc_ops proc_ip_ops = {
    .proc_open    = proc_ip_open,
    .proc_read    = seq_read,
    .proc_write   = proc_ip_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

int init_proc_ip(void) {
    proc_create("proc_ip", 0644, NULL, &proc_ip_ops);
    return 0;
}

void deinit_proc_ip(void) {
    remove_proc_entry("proc_ip", NULL);
}