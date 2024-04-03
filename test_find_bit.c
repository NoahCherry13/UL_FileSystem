#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BLOCKS 80
uint8_t free_bit_map[MAX_BLOCKS/8];


void set_bit(int block_num)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num/8;
  free_bit_map[map_index] = free_bit_map[map_index]|(1<<bit_to_set);
}

void reset_bit(int block_num)
{
  int bit_to_set = block_num % 8;
  int map_index = block_num/8;
  free_bit_map[map_index] = free_bit_map[map_index]&(~(1<<bit_to_set));
}

int find_free_bit()
{
  int index = 0;
  int bit_num = 0;
  int block_num = -1;
  for (int i = 0; i < MAX_BLOCKS/8; i++){
    if (free_bit_map[i]^0xFF){
      index = i;
      break;
    }
  }

  for (int i = 0; i < 8; i++){
    if ((free_bit_map[index]>>i)^1){
      bit_num = i;
      break;
    }
  }
  block_num = 8*index + bit_num;
  return block_num;
}

int main(){
  for (int i = 0; i < 9; i++){
    free_bit_map[i] = 255;
  }
  free_bit_map[9] = 1;
  int free_bit = find_free_bit();
  printf("Free Block Available at {%d}\n", free_bit);
  set_bit(free_bit);
  
  return 0;
}
