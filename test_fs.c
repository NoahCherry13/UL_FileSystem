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

  for(int i = 0; i < 33; i++){
    open_fd_list[i].is_used = 1;
    fs_open("testName");
  }
  return 0;
}
