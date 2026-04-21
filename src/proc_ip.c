#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include "ping_config.h"

static struct proc_dir_entry *proc_ip_entry;

static int proc_ip_show(struct seq_file *m, void *v) {
    seq_printf(m, "IPv4: %pI4\n", &ip_addr);
    return 0;
}

static int proc_ip_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_ip_show, NULL);
}

static ssize_t proc_ip_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    char buf[16];
    u8 parsed_ip[4];
    size_t len = count;

    if (len > sizeof(buf) - 1)
        len = sizeof(buf) - 1;

    if (copy_from_user(buf, ubuf, len))
        return -EFAULT;

    buf[len] = '\0';
    strim(buf);

    if (!in4_pton(buf, -1, parsed_ip, -1, NULL))
        return -EINVAL;

    memcpy(&ip_addr, parsed_ip, sizeof(ip_addr));
    *ppos += count;

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
    proc_ip_entry = proc_create("proc_ip", 0644, NULL, &proc_ip_ops);
    if (!proc_ip_entry)
        return -ENOMEM;

    return 0;
}

void deinit_proc_ip(void) {
    if (!proc_ip_entry)
        return;

    proc_remove(proc_ip_entry);
    proc_ip_entry = NULL;
}
