/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  diskput
*/

#include "helpers.h"
   

int main(int argc, char *argv[]){

  Program prog = DISKPUT;
  if (!check_args(prog, argc)){
    fprintf(stderr, "Usage Error: ./diskput <disk_name>.IMA <file_to_write>\n");
		exit(1);
  }
  
	int fd;
	struct stat sb;
 
  fd = open(argv[1], O_RDWR);
  if (fd == -1){
    fprintf(stderr, "Error: failed to open the disk image\n");
		exit(1);
  }
	if (fstat(fd, &sb) == -1){
    fprintf(stderr, "Error: failed to stat the disk image\n");
		exit(1);
  }
  
  // Pointer to starting pos of mapped memory
  unsigned char *p = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
  if (p == MAP_FAILED){
    fprintf(stderr, "Error: failed to map memory\n");
		exit(1);
  }
  
  // open file - confirm exists
    // if not , print "File not found." and exit 
    
  char *path = (char *)calloc(100, sizeof(char));
  char *fnameAndExt = (char *)calloc(100, sizeof(char));
  
  strcpy(path, argv[2]); 
    
  fnameAndExt = strrchr(path, '/');
  if (fnameAndExt == NULL) fnameAndExt = path;
  else fnameAndExt++;
  
  char *fname = (char *)calloc(8, sizeof(char));
  char *ext = (char *)calloc(3, sizeof(char));
  extract_fname_and_ext(fnameAndExt, fname, ext);
  
  FILE *in_ptr = fopen(fnameAndExt, "rb");
   
   
  if (!in_ptr){
     fprintf(stderr, "File not found.\n");
     exit(1);
  }

	struct stat in_sb; 
	if (stat(fnameAndExt, &in_sb) == -1){
    fprintf(stderr, "Error: failed to stat the input file\n");
		exit(1);
  } 
  
  // search for the subdir
    // if directory doesn't exist, print "The directory not found." and exit
  char *path_arr[32];
  int index = 0;  
  char *token = strtok(path, "/");
  
  while (token){
    path_arr[index++] = token;
    token = strtok(NULL, "/");
  }
  
  index--;
  
  int offset = ROOT_START;
  for (int i = 0; i<index; i++){
    offset = get_subdir_offset(p, path_arr[i], offset);
    if (offset == -1){
      fprintf(stderr, "The directory not found.\n");
      exit(1);
    }
  }
  
  // check free space on file system
  // compare to file size 
    //if greater, print "No enough free space in the disk image." and exit
    
  if (in_sb.st_size > get_free_sectors(p)*SECTOR_SZ){
    fprintf(stderr, "No enough free space in the disk image.\n");
    exit(1);
  }
    
  int first_log_cluster = get_next_free_log_cluster(p);
  time_t m_dt = in_sb.st_mtime;
   
  write_dir_entry(p, first_log_cluster, path, fname, ext, m_dt, in_sb.st_size, offset);
  
  copy_to_disk(p, in_ptr, offset, in_sb.st_size, first_log_cluster);
  
  munmap(p, 0);

}