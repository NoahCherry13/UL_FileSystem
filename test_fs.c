#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "disk.h"
#include "fs.h"

int main(){
  //-------------------------------------------//
  //              Disk Setup                   //
  //-------------------------------------------//
  char *disk_name = "test_disk";
  if(make_fs(disk_name)){
    printf("unplanned testing opportunity\n");
  }
  
  if(mount_fs("test_disk")){
    printf("fuck\n");
  }

  fs_create("Hello_World");
  //fs_delete("Hello_World");
  //fs_create("Hello_World");
  
  int fd = fs_open("Hello_World");
  if(fd == -1){
    printf("file open failed\n");
  }


  //-------------------------------------------//
  //              Writing                      //
  //-------------------------------------------//  
  
  int read_buf_size = 1000;
  int write_buf_size = 1000000;
  int ret = 0;
  
  char *mid_message = "hello my name is noah"; //21
  char *buf = malloc(write_buf_size);
  char *read_buf = malloc(read_buf_size);

  int ind = 0;
  for (int i = 0; i < 990; i++){
    buf[i] = 'a';
  }
  for (int i = 990; i < 990+21; i++){
    buf[i] = mid_message[ind];
    ind++;
  }
  for (int i = 990+21; i < write_buf_size; i++){
    buf[i] = 'b';
  }
  int write_res = fs_write(fd, buf, write_buf_size);
  if (write_res == -1 || write_res == 0){
    printf("write failed\n");
    return -1;
  }
  if(fs_close(fd)){
    printf("file open failed\n");
  }
  fd = fs_open("Hello_World");
  if(fd == -1){
    printf("file open failed\n");
  }
  //-------------------------------------------//
  //              Get File Size                //
  //-------------------------------------------//
  int file_size = fs_get_filesize(fd);
  printf("filesize: %d\n", file_size);
  
  //-------------------------------------------//
  //              Reading                      //
  //-------------------------------------------//
 
  printf("preparing to read\n");
  memset(read_buf, 0, read_buf_size);
  ret = fs_lseek(fd, 990);
  ret = fs_read(fd, read_buf, 21);
  if(ret != 21){
    printf("wrong ret value!\n");
    exit(0);
  }
  /*
  memset(read_buf, 0, read_buf_size);
  printf("read_buf:\n");
  for(int i = 0; i < read_buf_size; i++){
    printf("%c", read_buf[i]);
  }
  fs_read(fd, read_buf, 990);
  memset(read_buf, 0, read_buf_size);
  if (fs_read(fd, read_buf, 21) == -1){
    printf("Failed Read\n");
  }
  */  
  printf("read_buf:\n");
  for(int i = 0; i < read_buf_size; i++){
    printf("%c", read_buf[i]);
  }

  if(fs_truncate(fd, 5000)){
    printf("failed to truncate\n");
  }

  file_size = fs_get_filesize(fd);
  printf("filesize: %d\n", file_size);
  
  
  if(fs_close(fd)){
    printf("file open failed\n");
  }
  
  if(fs_delete("Hello_World")){
    printf("Failed to delete file\n");
  }
  return 0;
}
