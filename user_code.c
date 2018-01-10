#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#define BUFFER_SIZE 20
typedef struct processes {
	pid_t pid;
	pid_t ppid;
	int cpu;
	long status;
	int valid;
}process_data;

enum status
{
	TASK_RUNNING=0,
	TASK_INTERRUPTIBLE=1,
	TASK_UNINTERRUPTIBLE=2,
	__TASK_STOPPED=4,
	__TASK_TRACED=8,
	EXIT_DEAD=16,
	EXIT_ZOMBIE=32,
	EXIT_TRACE=EXIT_ZOMBIE | EXIT_DEAD,
	TASK_DEAD=64,
	TASK_WAKEKILL=128,
	TASK_WAKING=256,
	TASK_PARKED=512,
	TASK_NOLOAD=1024,
	TASK_NEW= 2048,
	TASK_STATE_MAX=4096,
	TASK_KILLABLE=TASK_WAKEKILL | TASK_UNINTERRUPTIBLE,
	TASK_STOPPED=TASK_WAKEKILL | __TASK_STOPPED,
	TASK_TRACED=TASK_WAKEKILL | __TASK_TRACED,
	TASK_IDLE=TASK_UNINTERRUPTIBLE | TASK_NOLOAD,
	TASK_NORMAL=TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE,
	TASK_ALL=TASK_NORMAL | __TASK_STOPPED | __TASK_TRACED,
	TASK_REPORT=TASK_RUNNING | TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE | __TASK_STOPPED | __TASK_TRACED | EXIT_ZOMBIE | EXIT_DEAD
}status;

char * get_status(enum status x)
{
	char *y;
	switch(x)
	{
		case TASK_RUNNING:   y="TASK_RUNNING";
				     break;
		case TASK_INTERRUPTIBLE: y= "TASK_INTERRUPTIBLE";
					 break;
		case TASK_UNINTERRUPTIBLE: y="TASK_UNINTERRUPTIBLE";
					   break;
		case __TASK_STOPPED: y="TASK_STOPPED";
				   break;
		case __TASK_TRACED: y="TASK_TRACED";
				  break;
		case EXIT_DEAD: y="EXIT_DEAD";
				break;
		case EXIT_ZOMBIE: y="EXIT_ZOMBIE";
				  break;
		case EXIT_TRACE: y="EXIT_ZOMBIE | EXIT_DEAD";
				 break;
		case  TASK_DEAD: y="TASK_DEAD";
				 break;
		case TASK_WAKEKILL: y="TASK_WAKEKILL";
				    break;
		case TASK_WAKING: y="TASK_WAKING";
				  break;
		case TASK_PARKED: y="TASK_PARKED";
				  break;
		case TASK_NOLOAD: y="TASK_NOLOAD";
				  break;
		case TASK_NEW: y="TASK_NEW";
			       break;
		case TASK_STATE_MAX: y="TASK_STATE_MAX";
				     break;
		case TASK_KILLABLE: y="TASK_WAKEKILL | TASK_UNINTERRUPTIBLE";
				    break;
		case TASK_STOPPED: y="TASK_WAKEKILL | __TASK_STOPPED";
				   break;
		case TASK_TRACED: y="TASK_WAKEKILL | __TASK_TRACED";
				  break;
		case TASK_IDLE: y="TASK_UNINTERRUPTIBLE | TASK_NOLOAD";
				break;
		case TASK_NORMAL: y="TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE";
				  break;
		case TASK_ALL: y="TASK_NORMAL | __TASK_STOPPED | __TASK_TRACED";
			       break;
		case TASK_REPORT: y="TASK_RUNNING | TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE | __TASK_STOPPED | __TASK_TRACED | EXIT_ZOMBIE | EXIT_DEAD";
				  break;
	}
	return y;

}


int main()
{
	int fd,bytes_read,flag=1;;
	char *buffer=" ";
	char *temp;
	int i;
	enum status proc_status;
	if((fd = open("/dev/process_list", O_RDONLY))<0)   //Open the character Device
	{
		printf("Unable to open the character device\n");
		exit(1);
	}
	while(flag)
	{
		process_data *data=malloc(sizeof(process_data)*BUFFER_SIZE);
		if((bytes_read = read(fd, data, sizeof(process_data)*BUFFER_SIZE))<0)   //Read data from character device
		{
			printf("Unable to read from the  character device\n");
			exit(1);
		}
		for(i=0;i<BUFFER_SIZE;i++)
		{
			if(data[i].valid!=1)
			{
				flag=0;
				break;
			}
			//proc_status=data[i].status;
			printf( "PID=%d PPID=%d CPU=%d STATE=%s\n",data[i].pid,data[i].ppid,data[i].cpu,get_status(proc_status=data[i].status));
		}
		free(data);
		data=NULL;
	}
	close(fd);
}
