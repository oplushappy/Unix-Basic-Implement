#include "apue.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define	BUFFSIZE	4096

int main(int argc,  char *argv[])
{
		int		n;
		// int BUFFSIZE = atoi(argv[1]);
		// char	buf[BUFFSIZE];
		// char  *buf = malloc(sizeof(char) *  BUFFSIZE);
		char	buf[BUFFSIZE];

		int fd = open("/dev/null",  O_RDWR );
		while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
			if (write(STDOUT_FILENO, buf, n) != n) {
				err_sys("write error");
			}		
			fsync(fd);
		}
				

		if (n < 0)
				err_sys("read error");

		exit(0);
}

