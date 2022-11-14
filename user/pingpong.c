#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int fd_f[2], fd_c[2];
	pipe(fd_f);
	pipe(fd_c);
	
	if(fork() == 0) { 
		char buf;
		read(fd_c[0], &buf, 1);
		printf("%d: received ping\n", getpid());
		write(fd_f[1], &buf, 1);
	} else { 
		write(fd_c[1], "!", 1);
		char buf;
		read(fd_f[0], &buf, 1);
		printf("%d: received pong\n", getpid());
		wait(0);
	}
	exit(0);
}
