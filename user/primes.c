#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void is_primes(int fd_father[2]){
    int fd_child[2];// 该进程的子进程的管道读写符
    pipe(fd_child);

    // 打印一定是素数的数（第一个写进来的数）
    int p = 0;
    read(fd_father[0], &p, sizeof(p));
    printf("prime %d\n", p);
    // 如果n是p的整数倍，那他一定不是素数，丢弃;否则，可能是素数，写给子进程。
    int n;
    int left = 0; // 剩下几个数字没有判断。
    while(read(fd_father[0], &n, sizeof(p)) != 0) {
        if(n % p != 0) {
            write(fd_child[1], &n, sizeof(n));
            left++;
        }
    }
    close(fd_father[0]);
    close(fd_child[1]);
    if(left == 0){ // 说明所有数都判断完了，为了避免最后一层还会执行后面的fork,避免出现多个0.
        exit(0);
    }
    //上面同理，先把自己进程的读端和子进程的写端用完，关掉，再开子进程。

    if(fork() == 0){ // 该进程的子进程
        is_primes(fd_child); // 子进程读取父进程进程写进来数字，并判断是否为素数：可能是素数的，写给孙进程然后递归调用，一定不是素数的，丢弃。
    }
    wait(0);
}

int main(int argc, char** atgv) {
    int fd_child[2]; // 子进程的管道读写端
    pipe(fd_child);

    // 先把数字写进子进程管道，并关掉写端。
    for(int i = 2; i <= 35; i++){
        write(fd_child[1], &i, sizeof(i));
    }
    close(fd_child[1]);

    // 创建子进程，只需要子进程的读端。
    if(fork() == 0) {  
        is_primes(fd_child); // 子进程读取父进程进程写进来数字，并判断是否为素数：可能是素数的，写给孙进程然后递归调用，一定不是素数的，丢弃。
    } 
    wait(0);
    exit(0);
}
