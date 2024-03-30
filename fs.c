#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "disk.h"

#define MAX_BLOCKS 8192
#define MAX_FILES 64

/* Datastructures for FS implementation                                   
 * Superblock                                                    
 *   -Extent Implementation                                              
 *                       
 */

//void ptr [pointer to inode list][int][int][int]
struct super_block {
  struct inode *inode_list; 
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
  uint16_t *inode_nums;
};

/* Globals                                                                                    
 *                                                                                            
 */
const int char_size = sizeof(uint8_t);
uint8_t free_bit_map[MAX_BLOCKS/8];
struct inode inode_list[64];

/* Helper Functions                                                                           
 *                                                                                            
 */
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


/* Management Routines
 *
 */

int make_fs(const char *disk_name)
{
  char *sb_buf;
  if (make_disk(disk_name)){
    printf("Failed to Make Disk\n");
    return -1;
  }
  
  if (open_disk(disk_name)){
    printf("Failed to Open Disk\n");
    return -1;
  }
  
  return 0;
}
