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
  fs_create("Hello_World");

  int fd = fs_open("Hello_World");
  if(fd == -1){
    printf("file open failed\n");
  }
    
  char buf[BLOCK_SIZE];
  memset(buf, 0, sizeof(buf));
  strcpy(buf, "hello world");
  int write_res = fs_write(fd, buf, 20);
  if (write_res == -1 || write_res == 0){
    printf("write failed\n");
    return -1;
  }

  memset(buf, 0, sizeof(buf));
  if (fs_read(fd, buf, BLOCK_SIZE) == -1){
    printf("Failed Read\n");
  }
  for(int i = 0; i < 20; i++){
    printf("%c", buf[i]);
  }
  if(fs_close(fd)){
    printf("file open failed\n");
  }
  
  
  if(fs_delete("Hello_World")){
    printf("Failed to delete file\n");
  }
  return 0;
}
