/*************************************************************************
> File Name: server.c
> Author: Michael Jay
> mail: xuchuanwork@foxmail.com
> Created Time: 2016年07月07日 星期四 21时56分06秒
> Function: 
 ************************************************************************/
#include "myhead.h"

#define BUFFSIZE 1024 
#define IP "127.0.0.1"
#define PORT 9888


void clearZ(int sig)
{
	while(waitpid(-1, NULL, WNOHANG)>0);
	return;
}

	
int main(int argc, char* argv[])
{
	int listenfd,connfd,real_read;
	pid_t pid;
	SAIN servaddr, cliaddr;
	socklen_t clilen;
	char buf[BUFFSIZE];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	
	Listen(listenfd, 10);

	//忽略SIGCHILD，避免僵尸
	signal(SIGCHLD, clearZ);

	clilen = sizeof(cliaddr);

	while(1)
	{
		printf("parent: Waiting....\n");
		connfd = accept(listenfd, (SA*) &cliaddr, &clilen);
		printf("Connection from [%s:%d] \n", inet_ntoa(cliaddr.sin_addr),
				ntohs(cliaddr.sin_port));

		if((pid=fork()) == 0)
		{
			close(listenfd);
			printf("child: Waiting....\n");
			while(1)
			{
				if((real_read = recv(connfd, buf, BUFFSIZE-1,0)) >0)
				{
						buf[real_read] = '\0';
						printf("connfd = %d, pid = %d, recv:%s\n", connfd, pid, buf);
						send(connfd, buf, real_read, 0);
						printf("child: Waiting....\n");
				}
			}
			close(connfd);
			exit(EXIT_SUCCESS);
		}
		else
		{
			//父进程
			//继续监听请求队列，而不需要处理客户请求
			//因此，关闭connfd
		//	printf("parent : pid = %d.\n", pid);
			close(connfd);
		}
	}

	close(listenfd);
	
    exit(0);
}
