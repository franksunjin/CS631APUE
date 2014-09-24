#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <libgen.h>

#ifndef BUFFSIZE
#define BUFFSIZE 32768
#endif

/*
 * Return code
 */
enum{
    SUCCESS,
    WRONG_USAGE,
    INPUT_FILE_NOT_FIND,
    INPUT_IS_A_DIRECTORY,
    OUTPUT_OPEN_FAIL,
    NOT_ENOUGH_SPACE,
    MMAP_ERROR
};

/* usage */
void
usage(){
    printf("usage: tcp <file> <target>\n");
}
/*
 * Copy first argv[0] to second arg[1]
 */
int
main(int argc, char const *argv[])
{
    int in,out=STDOUT_FILENO,out_opened=0;
    char * target;
    char * tmp;
    char * slash="/";
    struct stat in_st;
    if(argc!=3){
        usage();
        return WRONG_USAGE;
    }
    /*
     * Open input directly
     */
    if((in=open(argv[1], O_RDONLY))<0){
        perror(argv[1]);
        return INPUT_FILE_NOT_FIND;
    }else if(read(in, NULL, 0)==-1){
        perror(argv[1]);
        return INPUT_IS_A_DIRECTORY;
    }
    /*
     * Try open and create file
     */
    if(out_opened==0&&(out=open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0755))<0){
        if(errno!=EISDIR){
            perror(argv[2]);
            return OUTPUT_OPEN_FAIL;
        }
    }else {
        out_opened=1;
    }
    
    /*
     * Try directory
     */
    if(out_opened==0&&(out=open(argv[2], O_DIRECTORY))<0){
        perror(argv[2]);
        return OUTPUT_OPEN_FAIL;
    }else{
        /*
         * Attach a '/' after target dir then attach file name
         */
        target=(char*)malloc(strlen(argv[2])+strlen(argv[1])+2);
        strcat(target,argv[2]);
        strcat(target,slash);
        tmp=(char*)malloc(strlen(argv[1])+1);
        strcat(tmp, argv[1]);
        strcat(target, basename(tmp));
        /*
         * Create or open file in target dir
         */
        if(out_opened==0&&(out=open(target, O_RDWR|O_CREAT|O_TRUNC, 0755))<0){
            perror(target);
			return OUTPUT_OPEN_FAIL;
        }else out_opened=1;
        
    }
    /*
     * Make sure out file is opened
     */

    if(out_opened==1){
        fstat(in, &in_st);
        target=mmap(NULL, in_st.st_size, PROT_READ, MAP_SHARED, in, 0);
        if (target==(void*) -1){
            perror(argv[1]);
			return MMAP_ERROR;
        }
        if(ftruncate(out,in_st.st_size)==-1){
            perror(target);
			return NOT_ENOUGH_SPACE;
        }
        slash=mmap(NULL, in_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, out, 0);
        if (slash==(void*) -1){
            perror(argv[2]);
			return MMAP_ERROR;
        }
        /* 
         * No error can be catched 
         */
        memcpy(slash, target, in_st.st_size);
    }
    return SUCCESS;
}
