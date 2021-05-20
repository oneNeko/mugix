#include <csignal>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <wait.h>
void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated\n", pid);
	}

	//pid = wait(&stat);
	//printf("child %d terminated\n", pid);
	return;
}
