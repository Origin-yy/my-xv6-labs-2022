#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char **argvbuf = &argv[1]; // 用来保存每一行要执行的命令的固定参数
    char buf[2048]; // 用来存标准输入中每一行的字符串
    while(read(0, buf, 1)){
        
    }
    exit(0);
}
