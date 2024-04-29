#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "disk.h"
#include "fs.h"

int main(){
  char *disk_name = "test_disk";
  if(make_fs(disk_name)){
    printf("unplanned testing opportunity\n");
  }
  //if(umount_fs("test_disk")){
  //  printf("shit\n");
  //}
  if(mount_fs("test_disk")){
    printf("fuck\n");
  }
  //fs_create("Hello_World");


  //testing repeated create and delete
  for (int i = 0; i < 10; i++){
    fs_create("delete_this");
    fs_delete("delete_this");
  }
  printf("exiting loop\n");


  
  int fd = fs_open("Hello_World");
  if(fd == -1){
    printf("file open failed\n");
  }

  int read_buf_size = 1500;
  int write_buf_size = 2000;

  char *mid_message = "hello my name is noah"; //21
  char *buf = malloc(write_buf_size);
  char *read_buf = malloc(read_buf_size);

  for (int i = 0; i < 990; i++){
    buf[i] = 'a';
  }
  int ind = 0;
  for (int i = 990; i < 990+21; i++){
    buf[i] = mid_message[ind];
    ind++;
  }
  for (int i = 990+21; i < write_buf_size; i++){
    buf[i] = 'b';
  }
  
  
  int write_res = fs_write(fd, buf, write_buf_size);
  //printf("after write\n");
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
  //printf("after 2nd open\n");
  memset(read_buf, 0, read_buf_size);
  printf("read_buf:\n");
  for(int i = 0; i < read_buf_size; i++){
    printf("%c", read_buf[i]);
  }
  if (fs_read(fd, read_buf, read_buf_size) == -1){
    printf("Failed Read\n");
  }
  printf("read_buf:\n");
  for(int i = 0; i < read_buf_size; i++){
    printf("%c", read_buf[i]);
  }
  if(fs_close(fd)){
    printf("file open failed\n");
  }
  
  if(fs_delete("Hello_World")){
    printf("Failed to delete file\n");
  }
  return 0;
}
