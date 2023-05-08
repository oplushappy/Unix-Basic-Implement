#include "apue.h"
#include <sys/wait.h>
#include<unistd.h>
#include <stdio.h>
static void	sig_int(int);		/* our signal-catching function */
int main(void) {
	char	buf[MAXLINE];	/* from apue.h */
	pid_t	pid;
	int		status;
	char *array[100];

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error");

	printf("## ");	/* print prompt (printf requires %% to print %) */
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */
		
		char *argv = strtok(buf," ");
		array[0] = argv;
		int count = 1;
		while(argv) {
			argv = strtok(NULL," ");
			array[count++] = argv;
		}

		if(strcmp(array[0],"cd") == 0) {
			// printf("%s\n",array[1]);
			chdir(array[1]);
		} else {
			if ((pid = fork()) < 0) {
				err_sys("fork error");
			} else if (pid == 0) {		/* child */
				// execlp(buf, buf, (char *)0);
				execvp(buf,array);
				err_ret("couldn't execute: %s", buf);
				exit(127);
			}		/* parent */
			if ((pid = waitpid(pid, &status, 0)) < 0)
				err_sys("waitpid error");	
			}
		printf("## ");
	}	
	
	exit(0);
}

void sig_int(int signo)
{
	printf("interrupt\n%% ");
}

