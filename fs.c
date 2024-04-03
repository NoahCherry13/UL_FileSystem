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
#define BLOCK_SIZE = 4000

/* Datastructures for FS implementation                                   
 * Superblock                                                    
 *   -Extent Implementation                                              
 *                       
 */

//void ptr [pointer to inode list][int][int][int]
struct super_block {
  uint16_t used_block_bitmap_count;  
  uint16_t used_block_bitmap_offset;
  uint16_t inode_metadata_blocks;
  uint16_t inode_metadata_offset;
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
  uint32_t descriptor;
  uint16_t *inode_nums;
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
struct directory dir;



//-------------------------Helper Functions------------------------//
int find_bit_index(int block_num)
{
  int map_index = block_num/char_size;
  return map_index;
}

void set_bit(int block_num)
{
  int bit_to_set = block_num % char_size;
  int map_index = find_bit_index(block_num);
  free_bit_map[map_index] = free_bit_map[map_index]|(1<<bit_to_set);
}

void reset_bit(int block_num)
{
  int bit_to_set = block_num % char_size;
  int map_index = find_bit_index(block_num);
  free_bit_map[map_index] = free_bit_map[map_index]&(~(1<<bit_to_set));
}


//-------------------Management Routines-------------------------//

int make_fs(const char *disk_name)
{
  struct super_block *sb = malloc(sizeof(struct super_block));
  sb->used_block_bitmap_count = 0;
  sb->used_block_bitmap_offset = 1;
  sb->inode_metadata_blocks = 64;
  sb->inode_metadata_offset = 3;

  //void *write_buf = malloc;
  //memcpy(write_buf, sb, sizeof(struct super_block));
  if (make_disk(disk_name)){
    printf("Failed to Make Disk\n");
    return -1;
  }
  
  if (open_disk(disk_name)){
    printf("Failed to Open Disk\n");
    return -1;
  }

  //Write superblock
  if (block_write(0, (void *)sb)){
    printf("Block Write Failed\n");
    return -1;
  }

  //write inode_bitmap
  if (block_write(0, (void *)inode_list)){
    printf("Block Write Failed\n");
    return -1;
  }
  //write data bitmap
  if (block_write(0, (void *)free_bit_map)){
    printf("Block Write Failed\n");
    return -1;
  }
  return 0;
}

int mount_fs(const char *disk_name)
{
  struct super_block *sb = malloc(sizeof(super_block));
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
