#include <linux/init.h>
#include <linux/module.h>
#include<linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include<linux/slab.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 20
MODULE_LICENSE("DUAL BSD/GPL");

int count=0;
int value=0;
int val=0;
int callcount=0;
int openfns(struct inode *inode,struct file *filep);
int closefns(struct inode *inode,struct file *filep);
ssize_t readfns(struct file *filep,char *buff,size_t count,loff_t *offp );

typedef struct processes {
	pid_t pid;
	pid_t ppid;
	int cpu;
	long status;
	int  valid;
}process_data;
process_data *data;

//Keeps track of all the file operations in this character device
struct file_operations my_fops={
	.open= openfns,
	.read= readfns,
	.release=closefns
};

//Missolonus device with its file operations, permitions and name
static struct miscdevice process_list={
	.minor = MISC_DYNAMIC_MINOR,
	.name = "process_list",
	.fops = &my_fops,
	.mode=0666
};




// called when module is installed
int __init init_module()
{
	if(misc_register(&process_list)<0)
		printk(KERN_ALERT "Unable to register the device\n");
	return 0;
}
// called when module is removed
void __exit cleanup_module()
{
	misc_deregister(&process_list);
	return ;
}

//Called when Read call is invoked
ssize_t readfns(struct file *filep,char *buffer,size_t count,loff_t *offp )
{
	int i=0;
	struct task_struct *task;
	val=0;callcount++;
	if(callcount==1)// only executed for the first call to read 
	{
		for_each_process(task)
			val++;		//Counts the no of process running in the system

		data=kmalloc(sizeof(process_data)*val,GFP_KERNEL);
		for_each_process(task) 
		{

			data[i].pid=task->pid;
			data[i].ppid=task->parent->pid;
			data[i].cpu=task_cpu(task);
			data[i].status=task->state;
			data[i].valid=1;
			value++;i++;


		}
	}
	if((callcount*BUFFER_SIZE)>value)
	{
		/* function to copy kernel space buffer touser space*/
		if (copy_to_user(buffer,&data[(callcount-1)*BUFFER_SIZE],sizeof(process_data)*BUFFER_SIZE) != 0 )
			printk( "Kernel -> userspace copy failed!\n" );
		callcount=0;value=0;kfree(data);data=NULL;
		return sizeof(process_data)*(value%((callcount-1)*BUFFER_SIZE));
	}

	/* function to copy kernel space buffer to user space*/
	if (copy_to_user(buffer,&data[(callcount-1)*BUFFER_SIZE],sizeof(process_data)*BUFFER_SIZE) != 0 )
		printk( "Kernel -> userspace copy failed!\n" );
	return sizeof(process_data)*BUFFER_SIZE;
}

int openfns(struct inode *inode,struct file *filep)
{
	if (count) //Only one process can open character device at a time
		return -1;
	count++;// keeps count of no of devices pened thsi file	

	return 0;
}


int closefns(struct inode *inode,struct file *filep)
{
	callcount=0;
	value=0;
	kfree(data);data=NULL;
	if (count>0)	//decreases the no of process opened the character device
		count--;
	return 0;
}
