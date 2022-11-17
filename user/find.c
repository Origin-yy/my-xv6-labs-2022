#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 比较传进来的路径的结尾的文件名是否是要查找的文件名
void match(char *path, char *target){
    char* p;
    // 让p指向path最后一个/的后面一个字符
    for(p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    if(!strcmp(p, target)){
        printf("%s\n", path);
    }
}

// 查找输入的目录（包括其子目录和自己）中是否含有要查找的文件
void find(char* path, char* target){
    int fd;
    struct dirent entry;
    struct stat st;

    // 输入的path本身是不是要找的文件
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    match(path, target);

    // 获取path信息
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 根据path类型进行操作
    switch(st.type){
        // 如果path的是普通文件，什么都不做，直接返回（自己不是，又没有子目录）
        case T_DEVICE:
        case T_FILE:
            break;
        // 如果是目录，需要再查找其子目录
        case T_DIR :
            char buf[512];        // 用来存路径名（路径起点为参数输入的目录名）
            strcpy(buf, path);
            char *p = buf + strlen(buf);   // 在路径名后加‘/’
            *p++ = '/';
            while(read(fd, &entry, sizeof(entry)) == sizeof(entry)){
                if(entry.inum == 0) // inum为0的话是隐藏文件
                    continue;
                // 不在.和..递归
                if(strcmp(entry.name, ".") && strcmp(entry.name, "..")){
                    // 完善路径全名，将即将要查找的子目录加到path后面，以此作为递归调用里的新path
                    memmove(p, entry.name, sizeof(entry.name));
                    find(buf, target);
                    // 子目录查找完之后，去掉path最后的文件名
                    memset(p, '\0', strlen(entry.name));
                }
            }
            break;
    }
    close(fd);
}
int main(int argc, char** argv){
    if(argc < 3){
        find(".", argv[1]);
    } else {
        find(argv[1], argv[2]);
    }
    exit(0);
}
/* 来自知乎的分享
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text) {
	if(re[0] == '^')
		return matchhere(re+1, text);
	do{  // must look at empty string
		if(matchhere(re, text))
		return 1;
	}while(*text++ != '\0');
	return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text) {
	if(re[0] == '\0')
		return 1;
	if(re[1] == '*')
		return matchstar(re[0], re+2, text);
	if(re[0] == '$' && re[1] == '\0')
		return *text == '\0';
	if(*text!='\0' && (re[0]=='.' || re[0]==*text))
		return matchhere(re+1, text+1);
	return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
	do{  // a * matches zero or more instances
		if(matchhere(re, text))
			return 1; 
	}while(*text!='\0' && (*text++==c || c=='.'));
	return 0;
}

void find(char *path, char *target) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		if(match(target, path)) {
			printf("%s\n", path);
		}
		break;
	case T_DIR:
		// printf("%s\n", path);
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("find: cannot stat %s\n", buf);
				continue;
			}
	//   printf("%s\n", buf);
			if(strcmp(buf+strlen(buf)-2, "/.") != 0 && strcmp(buf+strlen(buf)-3, "/..") != 0) {
				find(buf, target);
			}
		}
		break;
	}
	close(fd);
}

int main(int argc, char *argv[])
{
	if(argc < 3){
		exit(0);
	}
	char target[512];
	target[0] = '/';
	strcpy(target+1, argv[2]);
	find(argv[1], target);
	exit(0);
}
*/