#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>  
#include <sys/wait.h>

int main(){
	int fd[2];	//pipe
	pid_t pid1,pid2;	//两个进程号
	char sendbuf[50],revbuf[50];	//读写数据
	pipe(fd);
	pid1=fork();

	if(pid1==0){	//如果是子进程一
		close(fd[0]);
		lockf(fd[1],1,0);
		sprintf(sendbuf,"Child process 1 is sending a message!");
		write(fd[1],sendbuf,50);
		lockf(fd[1],0,0);
	}
	else{
		pid2=fork();
		if(pid2==0){	//否则创建子进程二
			close(fd[0]);
			lockf(fd[1],1,0);
			sprintf(sendbuf,"Child process 2 is sending a message!");
			write(fd[1],sendbuf,50);
			lockf(fd[1],0,0);
		}
		else{
			close(fd[1]);	//父进程等待子进程写完数据再读
			waitpid(pid1,NULL,0);
			
			
			waitpid(pid2,NULL,0);
			read(fd[0],revbuf,50);
			printf("%s\n",revbuf);
			read(fd[0],revbuf,50);
			printf("%s\n",revbuf);
		}
	} 
	return 0;
}

