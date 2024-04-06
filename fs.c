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
uint8_t data_bitmap[MAX_BLOCKS/8];
uint8_t inode_bitmap[64];
struct fd open_fd_list[32];              // list of open files
struct inode inode_list[64];
struct directory dirs[64];
struct super_block *sb;
//-------------------------Helper Functions------------------------//
void set_bit(int block_num, uint8_t *bitmap)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num / 8;
  bitmap[map_index] = bitmap[map_index]|(1<<bit_to_set);
}

void reset_bit(int block_num, uint8_t *bitmap)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num / 8;
  bitmap[map_index] = bitmap[map_index]&(~(1<<bit_to_set));
}

int find_free_bit(uint8_t *bitmap)
{
  int index = 0;
  int bit_num = 0;
  int block_num = -1;
  
  //initialize bitmaps
  for (int i; i < MAX_BLOCKS/8; i++){
    if (bitmap[i]^0xFF){
      index = i;
      break;
    }
  }

  for (int i; i < MAX_FILES/8; i++){
    if (bitmap[i]^0xFF){
      index = i;
      break;
    }
  }
  
  for (int i = 0; i < 8; i++){
    if ((bitmap[index]>>bit_num)^1){
      bit_num = i;
      break;
    }
  }
  block_num = 8*index + bit_num;
  return block_num;
}

int find_inode(){

}

//-------------------Management Routines-------------------------//

int make_fs(const char *disk_name)
{
  char empty_blk[BLOCK_SIZE];
  memset(empty_blk, 0, BLOCK_SIZE);
  sb = malloc(sizeof(struct super_block));
  sb->dentries = 1;
  sb->inode_bitmap = 3;
  sb->data_bitmap = 2;
  sb->inode_table = 4;

  for(int i = 0; i < 4; i++){
    set_bit(i, data_bitmap);
  }

  for(int i = 0; i < MAX_FILES; i++){
    reset_bit(i, inode_bitmap);
  }

  for(int i = 4; i < MAX_FILES; i++){
    reset_bit(i, data_bitmap);
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
  memcpy(empty_blk, inode_list, MAX_FILES * sizeof(struct inode));
  if (block_write(3, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }
  
  //write data bitmap
  memset(empty_blk, 0, BLOCK_SIZE);
  memcpy(empty_blk, data_bitmap, MAX_BLOCKS / 8);
  if (block_write(1, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }

  //write inode bitmap
  memset(empty_blk, 0, BLOCK_SIZE);
  memcpy(empty_blk, inode_bitmap, MAX_BLOCKS / 8);
  if (block_write(1, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }
  
  
  // write dir entries
  //dirs = (struct directory *) malloc(MAX_FILES *sizeof(struct directory));
  for (int i = 0; i < MAX_FILES; i++){
    dirs[i].used = 0;
    dirs[i].inode = -1;
    strcpy(dirs[i].obj_name, "");
  }
  if (block_write(1, empty_blk)){
    printf("Block Write Failed\n");
    return -1;
  }  
  return 0;
}

int mount_fs(const char *disk_name)
{
  /* This function mounts a file system that is stored on a virtual disk with name disk_name. 
   * With the mount operation, a file system becomes "ready for use." You need to open the 
   * disk and then load the meta-information that is necessary to handle the file system operations 
   * that are discussed below. The function returns 0 on success, and -1 when the disk disk_name 
   * could not be opened or when the disk does not contain a valid file system (that you previously 
   * created with make_fs). 
   */
  //possibly not needed
  if (open_disk(disk_name)){
    printf("failed to open disk partition\n");
    return -1;
  }

  char read_buffer[BLOCK_SIZE];  //block size buffer to read from disk
  sb = malloc(sizeof(struct super_block));
  
  if (block_read(0, read_buffer)){
    printf("Failed to Read Block\n");
    return -1;
  }

  memcpy(sb, read_buffer, sizeof(struct super_block));

  uint16_t data_map_offset = sb->data_bitmap;
  uint16_t inode_map_offset = sb->inode_bitmap;
  uint16_t inode_list_offset = sb->inode_table;
  uint16_t dentry_offset = sb->dentries;

  //read in bitmaps
  if (block_read(data_map_offset, read_buffer)){
    printf("Failed to Read Block\n");
    return -1;
  }
  memcpy(data_bitmap, read_buffer, sizeof(data_bitmap));

  if (block_read(inode_map_offset, read_buffer)){
    printf("Failed to Read Block\n");
    return -1;
  }
  memcpy(inode_bitmap, read_buffer, sizeof(inode_bitmap));
  
  if (block_read(inode_list_offset, read_buffer)){
    printf("Failed to Read Block\n");
    return -1;
  }
  memcpy(inode_list, read_buffer, sizeof(inode_list));

  if (block_read(dentry_offset, read_buffer)){
    printf("Failed to Read Block\n");
    return -1;
  }
  memcpy(dirs, read_buffer, sizeof(dirs));

  for (int i = 0; i < 32; i++){
    open_fd_list[i].is_used = 0;
    open_fd_list[i].offset = 0;
  }
  
  return 0;
}

int umount_fs()
{
  char write_buffer[BLOCK_SIZE];
  memset(write_buffer, 0, BLOCK_SIZE);
  memcpy(write_buffer, dirs, MAX_FILES*sizeof(struct directory));

  if (block_write(sb->dentries, write_buffer)){
    printf("Error Writing Block\n");
    return -1;
  }

  memset(write_buffer, 0, BLOCK_SIZE);
  memcpy(write_buffer, data_bitmap, sizeof(data_bitmap));
  if (block_write(sb->data_bitmap, write_buffer)){
    printf("Error Writing Block\n");
    return -1;
  }  

  memset(write_buffer, 0, BLOCK_SIZE);
  memcpy(write_buffer, inode_bitmap, sizeof(inode_bitmap));
  if (block_write(sb->inode_bitmap, write_buffer)){
    printf("Error Writing Block\n");
    return -1;
  }

  memset(write_buffer, 0, BLOCK_SIZE);
  memcpy(write_buffer, inode_list, sizeof(inode_list));
  if (block_write(sb->inode_table, write_buffer)){
    printf("Error Writing Block\n");
    return -1;
  }

  for(int i = 0; i < MAX_FILES; i++){
    open_fd_list[i].is_used = 0;
    open_fd_list[i].offset = 0;
  }

  if (close_disk()){
    printf("Failed to close disk\n");
    return -1;
  }
  
  return 0;
}

int fs_open(const char *name){
  
}
