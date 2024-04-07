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
  umount_fs("test_disk");
  mount_fs("test_disk");
  fs_create("Hello_World");
  // fs_delete("Hello_World");
  return 0;
}
