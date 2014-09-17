#!/bin/sh
#test tcp and tcpm
mkdir test_tcp
mkdir test_tcp/dir/
mkdir  test_tcp/dir/filedir/
echo "this is test" > test_tcp/1.txt
echo "this is test" > this.txt
./$1 test_tcp/1.txt 
./$1 test_tcp/1.txt test_tcp/2.txt sdd
./$1 test_tcp/1.txt test_tcp/2.txt
./$1 test_tcp/1.txt test_tcp/dir/2.txt
./$1 test_tcp/1.txt test_tcp/dir/filedir/2.txt
./$1 test_tcp/1.txt test_tcp/dir/
./$1 test_tcp/1.txt test_tcp/dir
./$1 this.txt test_tcp/dir/
./$1 this.txt test_tcp/dir
./$1 test_tcp/1.txt test_tcp/disr/
./$1 test_tcp/1d.txt test_tcp/dir
#rm -rf $PWD/test_tcp
rm -rf this.txt
