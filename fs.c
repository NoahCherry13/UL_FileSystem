#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "disk.h"

#define MAX_BLOCKS 8192
#define MAX_FILES 64
#define MAX_FILE_SIZE 1000000


/* Datastructures for FS implementation                                   
 * Superblock                                                    
 *   -Extent Implementation                                              
 *                       
 */

//void ptr [pointer to inode list][int][int][int]
struct super_block {
  uint16_t dentries;  
  uint16_t data_bitmap;
  uint16_t inode_bitmap;
  uint16_t inode_table;
};

struct inode {
  uint16_t magic_number;
  uint16_t direct_offset;
  //uint16_t single_indirect_offset
  //uint16_t double_indirect_offset
  uint16_t file_size;
};

struct directory{
  char obj_name[16];
  char used;
  uint16_t inode;
};

struct fd{
  char is_used;
  uint32_t inode_num;
  uint32_t offset;
};


//-------------------------- Globals--------------------------------//                                                                                  
const int char_size = sizeof(uint8_t);
uint8_t free_bit_map[MAX_BLOCKS/8];
uint32_t open_fd_list[32];              // list of open files
struct inode inode_list[64];
struct directory *dir;

//-------------------------Helper Functions------------------------//
void set_bit(int block_num)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num / 8;
  free_bit_map[map_index] = free_bit_map[map_index]|(1<<bit_to_set);
}

void reset_bit(int block_num)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num / 8;
  free_bit_map[map_index] = free_bit_map[map_index]&(~(1<<bit_to_set));
}

int find_free_bit()
{
  int index = 0;
  int bit_num = 0;
  int block_num = -1;
  for (int i; i < MAX_BLOCKS/8; i++){
    if (free_bit_map[i]^0xFF){
      index = i;
      break;
    }
  }

  for (int i = 0; i < 8; i++){
    if ((free_bit_map[index]>>bit_num)^1){
      bit_num = i;
      break;
    }
  }
  block_num = 8*index + bit_num;
  return block_num;
}
//-------------------Management Routines-------------------------//

int make_fs(const char *disk_name)
{
  char empty_blk[BLOCK_SIZE];
  memset(empty_blk, 0, BLOCK_SIZE);
  struct super_block *sb = malloc(sizeof(struct super_block));
  sb->dentries = 1;
  sb->inode_bitmap = 3;
  sb->data_bitmap = 2;
  sb->inode_table = 4;

  for(int i = 0; i < 4; i++){
    set_bit(i);
  }
  for(int i = 4; i < MAX_FILES; i++){
    reset_bit(i);
  }
  if (make_disk(disk_name)){
    printf("Failed to Make Disk\n");
    return -1;
  }
  
  if (open_disk(disk_name)){
    printf("Failed to Open Disk\n");
    return -1;
  }

  //Write superblock
  memcpy(empty_blk, sb, sizeof(struct super_block));
  if (block_write(0, (void *)sb)){
    printf("Block Write Failed\n");
    return -1;
  }

  //write inode list
  memset(empty_blk, 0, BLOCK_SIZE);
  memcpy(empty_blk, inode_list, MAX_BLOCKS * sizeof(struct inode));
  if (block_write(3, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }
  
  //write data bitmap
  memset(empty_blk, 0, BLOCK_SIZE);
  memcpy(empty_blk, free_bit_map, MAX_BLOCKS / 8);
  if (block_write(1, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }
  
  // write dir entries
  dir = (struct directory *) malloc(MAX_FILES *sizeof(struct directory));
  for (int i = 0; i < MAX_FILES; i++){
    dir[i].used = 0;
    dir[i].inode = -1;
    strcpy(dir[i].obj_name, "");
  }
  if (block_write(1, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }  
  return 0;
}

int mount_fs(const char *disk_name)
{
  struct super_block *sb = malloc(sizeof(struct super_block));
  /* This function mounts a file system that is stored on a virtual disk with name disk_name. 
   * With the mount operation, a file system becomes "ready for use." You need to open the 
   * disk and then load the meta-information that is necessary to handle the file system operations 
   * that are discussed below. The function returns 0 on success, and -1 when the disk disk_name 
   * could not be opened or when the disk does not contain a valid file system (that you previously 
   * created with make_fs). 
   */
  if (open_disk(disk_name)){
    printf("failed to open disk partition\n");
    return -1;
  }

  if (block_read(0, (void*)sb)){
    printf("Failed to Read Block\n");
    return -1;
  }

  

  
  
  return 0;
}
