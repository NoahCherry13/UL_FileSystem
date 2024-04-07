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

  char buf[1];
  if (fs_read(fd, buf, 10)){
    printf("Failed Read\n");
  }

  if(fs_close(fd)){
    printf("file open failed\n");
  }
  
  
  if(fs_delete("Hello_World")){
    printf("Failed to delete file\n");
  }
  return 0;
}
