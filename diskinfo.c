/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  diskinfo
*/

#include "helpers.h"

int main(int argc, char *argv[])
{
  Program prog = DISKINFO;
  if (!check_args(prog, argc)){
    fprintf(stderr, "Usage Error: ./diskinfo <disk_name>.IMA\n");
		exit(1);
  }
  
	int fd;
	struct stat sb;

	fd = open(argv[1], O_RDONLY);
  if (fd == -1){
    fprintf(stderr, "Error: failed to open the disk image\n");
		exit(1);
  }
	if (fstat(fd, &sb) == -1){
    fprintf(stderr, "Error: failed to stat the disk image\n");
		exit(1);
  }
  
  // Pointer to starting pos of mapped memory
  unsigned char * p = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0); 
  if (p == MAP_FAILED){
    fprintf(stderr, "Error: failed to map memory\n");
		exit(1);
  }
  
  const char *labels[NUM_LABELS] = {"OS Name", "Label of the disk", "Total size of the disk", "Free size of the disk", "The number of files in the image", "Number of FAT copies", "Sectors per FAT"};
  
  
  for (int i = 0; i < NUM_LABELS; i++){
  
    char * value = (char *) malloc(sizeof(char) * BUFFER);
   
    switch (i){
      case 0:
        get_string(p, value, OS_NAME_START, OS_NAME_LEN +1);
        printf("%s: %s\n", labels[i], value);
        break;
      case 1:
        get_vol_label(p, value);
        printf("%s: %s\n", labels[i], value);
        break;
      case 2:
        printf("%s: %d bytes\n", labels[i], get_int(p, TOT_SZ_START, TOT_SZ_LEN)*SECTOR_SZ);
        break;
      case 3:
        printf("%s: %d bytes\n", labels[i], get_free_sectors(p)*SECTOR_SZ);
        break;
      case 4:
        printf("\n==============\n");
        printf("%s: %d\n", labels[i], get_num_files(p, ROOT_START));
        break;
      case 5:
        printf("\n==============\n");
        get_string(p, value, NUM_FAT_START, NUM_FAT_LEN);
        printf("%s: %d\n", labels[i], (int)(*value));
        break;
      case 6:
        get_string(p, value, SPFAT_START, SPFAT_LEN);
        printf("%s: %d\n", labels[i], (int)(*value));
        break;
    }
    free(value);
  }
  munmap(p, 0);
}