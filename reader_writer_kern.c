#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define ITERATIONS 1000

static struct task_struct *readers[NUM_READERS];
static struct task_struct *writers[NUM_WRITERS];

static int shared_data = 0;
static DECLARE_RWSEM(rwsem);

static int reader_fn(void *data)
{
	int id = (int)(long)data;
	int i;

	for (i = 0; i < ITERATIONS; i++) {
		down_read(&rwsem);
		pr_info("Reader %d reads %d\n", id, shared_data);
		up_read(&rwsem);
		msleep(1);
		if (kthread_should_stop())
			break;
	}
	return 0;
}

static int writer_fn(void *data)
{
	int id = (int)(long)data;
	int i;

	for (i = 0; i < ITERATIONS; i++) {
		down_write(&rwsem);
		shared_data++;
		pr_info("Writer %d writes %d\n", id, shared_data);
		up_write(&rwsem);
		msleep(1);
		if (kthread_should_stop())
			break;
	}
	return 0;
}

static int __init rw_module_init(void)
{
	int i;

	pr_info("RW module: init\n");

	for (i = 0; i < NUM_READERS; i++)
		readers[i] = kthread_run(reader_fn, (void *)(long)i, "reader_%d", i);

	for (i = 0; i < NUM_WRITERS; i++)
		writers[i] = kthread_run(writer_fn, (void *)(long)i, "writer_%d", i);

	return 0;
}

static void __exit rw_module_exit(void)
{
	int i;

	pr_info("RW module: exit\n");

	for (i = 0; i < NUM_READERS; i++)
		if (readers[i])
			kthread_stop(readers[i]);

	for (i = 0; i < NUM_WRITERS; i++)
		if (writers[i])
			kthread_stop(writers[i]);
}

module_init(rw_module_init);
module_exit(rw_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KGD");
MODULE_DESCRIPTION("Reader-Writer example with kthread");
