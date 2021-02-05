/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  disklist
*/

#include "helpers.h"
#include "DirList.h"

/*
*  Recursive function to print the dir/file listings in the given directory and all subdirectories
*/
void print_listing(unsigned char *p, Dirs *dirs){
  
  if (!dirs){
    // DONE
    return;
  }
  
  int offset = dirs->index;
  int fatEntry = dirs->FAT;
  char path[100];
  strcpy(path, dirs->path);
  trim_whitespace(path);

  if (strcmp(path, "")) printf("\n%s\n==================\n", path);
  
  remove_dir(&dirs);
  
  int num_subdirs = get_num_files_and_dirs(p, offset);

  int next_log_cluster = get_nth_fat_entry(p, fatEntry);
  
  if (next_log_cluster < 0xFF0){
    num_subdirs += get_num_files_and_dirs(p, map_to_phys(next_log_cluster));
    next_log_cluster = get_nth_fat_entry(p, next_log_cluster);
  }
    
  for (int i=0; i<num_subdirs; i++){
    
    char type;
    int fsize;
    char fname[20];
    char dt[20];
    
    
    while (TRUE){
      if (is_file(p, offset)) break;
      offset += DIR_ENTRY_LEN;

      if (fatEntry != 0 && offset % SECTOR_SZ == 0){
        int next_log_cluster = get_nth_fat_entry(p, fatEntry);
        
        if (next_log_cluster < 0xFF0){
          offset = map_to_phys(next_log_cluster);
          fatEntry = next_log_cluster;
        }
      }
    }

    
    if (p[offset] == '.'){
      offset += 32;
      continue;
    }
    
    type = get_type(p, offset);

    fsize = get_int(p, offset + FILE_SZ_START, FILE_SZ_LEN);
    get_string(p, fname, offset, FNAME_LEN);
    trim_whitespace(fname);
    
    if (type == 'D') {
      int first_log_cluster = get_first_log_cluster(p, offset);
      char subpath[100];
      strncpy(subpath, path, 100);
      strcat(subpath, "/");
      strcat(subpath, fname);
      
      add_dir(&dirs, first_log_cluster, (first_log_cluster + MAP_TO_PHYS) * SECTOR_SZ, subpath);
          
    }
    else {
      // add extention
      char ext[4];
      get_string(p, ext, offset + EXT_START, EXT_LEN);
      trim_whitespace(ext);
      if (strcmp(ext, "")){
        strcat(fname, ".");
        strcat(fname, ext);
      }
    }
    
    get_date_time(p + offset, dt);
    
    // print the dir/file info    
    printf(DIRSTR, type, fsize, fname, dt);
        
    offset += 32;
  }
  
  print_listing(p, dirs);
}  


int main(int argc, char *argv[]){

  Program prog = DISKLIST;
  if (!check_args(prog, argc)){
    fprintf(stderr, "Usage Error: ./disklist <disk_name>.IMA\n");
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
   
  printf("Root\n==================\n");
  Dirs *dirs;
  Dirs dir = {0, ROOT_START, "\0", NULL};
  
  dirs = &dir;

  print_listing(p, dirs);  
  
  munmap(p, 0);
}
