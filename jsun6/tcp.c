#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "fcntl.h"
#include "string.h"
#include "errno.h"
#include "locale.h"
#include "sys/stat.h"
#include "string.h"
#include "libgen.h"

#ifndef BUFFSIZE
#define BUFFSIZE 32768
#endif

/* usage */
void usage(){
    printf("usage: tcp <file> <target>\n");
}

int main(int argc, char const *argv[])
{
    int in,out=STDOUT_FILENO,out_opened=0;
    int n;
    char * target;
    char * slash="/";
	char buf[BUFFSIZE];
    struct stat in_st;
    if(argc!=3){
        usage();
        return 1;
    }
    /* open directly */
    if((in=open(argv[1], O_RDONLY))<0){
        perror(argv[1]);
        return 1;
    }
    
    /* open directly */
    if(out_opened==0&&(out=open(argv[2], O_WRONLY))<0){
    }else {
        out_opened=1;
    /* clear file and catch error */
        if(truncate(argv[2],0)==-1){
            perror(argv[2]);
			return 1;
        }
    }
    /* try create */
    if(out_opened==0&&(out=open(argv[2], O_WRONLY|O_CREAT))<0){
        
    }else out_opened=1;
    
    /* try directory */
    if(out_opened==0&&(out=open(argv[2], O_DIRECTORY))<0){
        perror(argv[2]);
        return 1;
    }else{
        /* see if out path have '/' in the end */
        if(argv[2][strlen(argv[2])-1]=='/'){
            target=(char*)malloc(strlen(argv[2])+strlen(argv[1])+1);
            strlcat(target,argv[2],strlen(argv[2])+1);
        }else{
            target=(char*)malloc(strlen(argv[2])+strlen(argv[1])+2);
            strlcat(target,argv[2],strlen(argv[2])+1);
            strlcat(target,slash,strlen(argv[2])+2);
        }
        strcat(target, basename(argv[1]));
        /* this part should be a function */
        if(out_opened==0&&(out=open(target, O_WRONLY))<0){
        }else {
            out_opened=1;
            truncate(argv[2],0);
        }
        if(out_opened==0&&(out=open(target, O_WRONLY|O_CREAT))<0){
            perror(target);
			return 1;
        }else out_opened=1;
        
    }
    /* make sure out file is opened */
    if(out_opened==1){
        fstat(in, &in_st);
        
    /* truncate the target file and catch error */
        if(ftruncate(out,in_st.st_size)==-1){
            perror(target);
			return 1;
        }
        while ((n = (int)read(in, buf, BUFFSIZE)) > 0) {
            if (write(out, buf, n) != n) {
    /* catch write error*/
                perror(target);
                return 1;
            }
        }
    }
    /* catch read error*/
    if(n<0){
        perror(target);
        return 1;
    }
    return 0;
}
