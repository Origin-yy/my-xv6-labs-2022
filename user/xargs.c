#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 传入程序名和参数列表，执行这个程序
void run(char *program, char **args) {
	if(fork() == 0) { 
		exec(program, args);
		exit(0);
	}
	return; 
}

int main(int argc, char *argv[]){
	char buf[2048];                     // 从stdin读字符时的缓冲池
	char *buf_begin = buf,*buf_p = buf; // 指向当前正在读取的参数的开始指针和某个字符的移动指针
	char *argsbuf[128];                 // 一条命令行的全部参数列表，包括xargs后面自带的固定的参数和stdin读入的参数
	char **input_begin = argsbuf;       // 指向 argsbuf 中第一个从 stdin 读入的参数，便于读入新的一行
    // 将xargs后面自带的固定参数写入参数列表
	for(int i=1;i<argc;i++) {     
		*input_begin = argv[i];
		input_begin++;
	}
	char **argsbuf_p = input_begin;  // 指向 agrsbuf 内字符的指针，用它来赋值
	while(read(0, buf_p, 1) != 0) {
		if(*buf_p == ' ' || *buf_p == '\n') {
            // 读入一个参数完成（以空格分隔，如 `echo hello world`，则 hello 和 world 各为一个参数）
            // 将空格替换为 \0 分割开各个参数，这样可以直接使用内存池中的字符串作为参数字符串，而不用额外开辟空间
			*buf_p = '\0';
			*(argsbuf_p++) = buf_begin; // 全部参数列表加入这个参数
			buf_begin = buf_p+1;        // 下一个参数的起始位置在下一个位置处
            // 如果一行参数读入完成，运行程序
			if(*buf_p == '\n') {
				*argsbuf_p = 0;          // 参数列表末尾用 null 标识列表结束
				run(argv[1], argsbuf); 
				argsbuf_p = input_begin; // 重置读入参数指针，准备读入下一行
			}
		}
		buf_p++;  // 既不是空格也不是回车，读入后指向下一个位置。
	}
    // 最后一行参数结尾没有回车，如果最后一行不是空行，需要加入最后一行参数运行程序
	if(argsbuf_p != input_begin) {
		*buf_p = '\0';
		*(argsbuf_p++) = buf_begin;
		*argsbuf_p = 0; // 参数列表用null标识列表结束
		run(argv[1], argsbuf);
	}
	while(wait(0) != -1);// 循环等待所有子进程完成，每一次 wait(0) 等待一个
	exit(0);
}
