/*
 * Jin Sun ID 10383476
 * email: franksunjin@gmail.com
 * 09/17/2014
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <sys/stat.h>
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
    WRITE_ERROR,
    READ_ERROR
};
/*
 * usage
 */
void
usage(){
    printf("usage: tcp <file> <target>\n");
}
/*
 * Copy first arg to second arg
 */
int
main(int argc, char const *argv[])
{
    int in,out=STDOUT_FILENO,out_opened=0;
    int n;
    char * target;
    char * tmp;
    char * slash="/";
	char buf[BUFFSIZE];
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
    if(out_opened==0&&(out=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0755))<0){
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
     */    if(out_opened==1){
        fstat(in, &in_st);
        /*
         * Truncate the target file and catch error
         */
        if(ftruncate(out,in_st.st_size)==-1){
            perror(target);
			return NOT_ENOUGH_SPACE;
        }
        while ((n = (int)read(in, buf, BUFFSIZE)) > 0) {
            if (write(out, buf, n) != n) {
                /*
                 * Catch write error
                 */
                perror(target);
                return WRITE_ERROR;
            }
        }
    }
    /*
     * Catch read error
     */
    if(n<0){
        perror(target);
        return READ_ERROR;
    }
    return SUCCESS;
}
