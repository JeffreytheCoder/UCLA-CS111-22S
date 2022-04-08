#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static int proc_count_show(struct seq_file *m, void *v)
{
	struct task_struct *p;
	int count = 0;	

	for_each_process(p) {
		count++;
	}
	seq_printf(m, "%d\n", count);

	return 0;
}

static int proc_count_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_count_show, NULL);
}

static const struct proc_ops proc_count_ops = {
	.proc_open = proc_count_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");

	proc_create("count", 0, NULL, &proc_count_ops);
	return 0;
}

static void __exit proc_count_exit(void)
{
	pr_info("proc_count: exit\n");
	remove_proc_entry("count", NULL);
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Your name");
MODULE_DESCRIPTION("One sentence description");
MODULE_LICENSE("GPL");
