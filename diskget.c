/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  diskget
*/

#include "helpers.h"


int main(int argc, char *argv[]){

  Program prog = DISKGET;
  if (!check_args(prog, argc)){
    fprintf(stderr, "Usage Error: ./diskget <disk_name>.IMA <file_to_copy>\n");
		exit(1);
  }
  
	int fd;
	struct stat sb;
  char *fname = (char *)calloc(8, sizeof(char));
  char *ext = (char *)calloc(3, sizeof(char));
  int f_offset = -1;

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
  unsigned char *p = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0); 
  if (p == MAP_FAILED){
    fprintf(stderr, "Error: failed to map memory\n");
		exit(1);
  }

  extract_fname_and_ext(argv[2], fname, ext);
  f_offset = get_file_offset(p, fname, ext);
  
  if (f_offset == -1 || get_type(p, f_offset) == 'D'){
    fprintf(stderr, "File not found.\n");
		exit(1);
  }
 
  copy_file(p, f_offset, argv[2]);
  
  munmap(p, 0);
  
}