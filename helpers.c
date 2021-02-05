/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  helper functions
*/

#include "helpers.h"

/*
*  A function to get the length of a string 
*/
int get_len(char *str){
  int count = 0;
  char c = str[0];
  while (c) c = str[++count];
  return count;
}

/*
*  A function to extract file name and extention components from a string
*/
void extract_fname_and_ext(char *str, char *fname, char *ext){

  int str_len = get_len(str);
  int indexOfDot = 0;
  int i;
  
  for (i = 0; i < FNAME_LEN && i < str_len; i++){
    if (str[i] == '.'){
      indexOfDot = i;
      break;
    }
    else fname[i] = str[i];
  }

  if (indexOfDot == 0 && str_len > FNAME_LEN){
    if (str[FNAME_LEN] == '.') indexOfDot = FNAME_LEN;
  }
  
  if (indexOfDot > 0){
    for (i = 0; i < EXT_LEN && i < str_len; i++){
      ext[i] = str[indexOfDot + i + 1];
    }
  }
}


/*
*  Searches root of disk image dp for the given file.ext
*  Returns the root offset for the file, or -1 if not found
*/
int get_file_offset(unsigned char *dp, char *fname, char *ext){
  int offset = -1;
  
  for (int i = ROOT_START; i < ROOT_END; i += DIR_ENTRY_LEN) {
    if (dp[i] == 0x00) break;
    
    char *cur_name = (char *)calloc(FNAME_LEN, sizeof(char));
    get_string(dp, cur_name, i, FNAME_LEN);
    trim_whitespace(cur_name);
    
    if (strcmp(cur_name, fname)) continue; // not the correct filename
        
    char *cur_ext = (char *)calloc(EXT_LEN, sizeof(char));
    get_string(dp, cur_ext, i + EXT_START, EXT_LEN);
    trim_whitespace(cur_ext);
    
    if (strcmp(cur_ext, ext)) continue; // not the correct extention
    
    offset = i;
    break;
  }

  return offset;
}

/*
*  Copies the file at offset in disk image dp to the current directory with file name path
*/
void copy_file(unsigned char *dp, int offset, char *path){
  FILE *out_ptr = fopen(path, "wb");
  int fsize = get_int(dp,  offset + FILE_SZ_START, FILE_SZ_LEN); 
  int log_sec = get_first_log_cluster(dp, offset);
  int phys_sec = map_to_phys(log_sec);
  
  if (!out_ptr){
    fprintf(stderr, "Error: failed to write to %s\n", path);
    exit(1);
  }
  
  for(int i = 0; i<fsize; i+=SECTOR_SZ) {
    
    int sz = SECTOR_SZ;
    if (fsize - i < SECTOR_SZ) sz = fsize - i;
    
    char *buf = (char *)calloc(sz, sizeof(char));
    get_string(dp, buf, phys_sec, sz);
    
    fwrite(buf, sizeof(char), sz, out_ptr);
    
    log_sec = get_nth_fat_entry(dp, log_sec);
    phys_sec = map_to_phys(log_sec);
    
  }
  fclose(out_ptr);  
}

  
/*
*  Trims the whitespace 0x20 chars from the end of a string
*/
void trim_whitespace(char str[]){
  int i = 0;
  while(str[i] != 0x20 && str[i] != '\0') i++;
  str[i] = '\0';
}

/* 
*  This function was provided by the CSC 360 TA
*  Extracts the creation date/time for a file
*/
void get_date_time(unsigned char * directory_entry_startPos, char * dt){
	
	int time, date;
	int hours, minutes, day, month, year;
	
	time = *(unsigned short *)(directory_entry_startPos + TIME_OFFSET);
	date = *(unsigned short *)(directory_entry_startPos + DATE_OFFSET);
	
	//the year is stored as a value since 1980
	//the year is stored in the high seven bits
	year = ((date & 0xFE00) >> 9) + 1980;
	//the month is stored in the middle four bits
	month = (date & 0x1E0) >> 5;
	//the day is stored in the low five bits
	day = (date & 0x1F);
	
	//the hours are stored in the high five bits
	hours = (time & 0xF800) >> 11;
	//the minutes are stored in the middle 6 bits
	minutes = (time & 0x7E0) >> 5;
	
	sprintf(dt, "%d-%02d-%02d %02d:%02d", year, month, day, hours, minutes);

	return ;	
}

/* 
 * Confirms the correct number of arguments are provided for the given cmd
 */ 
int check_args(Program prog, int argc){
  int retVal = FALSE;
  switch(prog){
    case DISKINFO:
    case DISKLIST:
      if (argc == 2) retVal =TRUE;
      break;
    case DISKGET:
    case DISKPUT:
      if (argc == 3) retVal = TRUE;
      break;
  }
  return retVal;
}


/* 
 * Gets the string of characters stored in disk image 'dp'
 * starting at index 'start' and stores then in 'retStr'
 */ 
void get_string(unsigned char *dp, char* retStr, int start, int len){
  for (int i = 0; i < len; i++){
    retStr[i] = dp[i + start];
  }
  retStr[len] = '\0';
}

/* 
 * power function: returns base^exp
 */ 
int to_power(int base, int exp){
  if (exp == 0){
    return 1;
  }
  else{
    return base * to_power(base, exp-1);
  }
}

/* 
 * returns the integer stored in disk image 'dp'
 * starting at index 'start' 
 */ 
int get_int(unsigned char *dp, int start, int len){
  int retVal=0;
  int power = 0;
  for (int i = 0; i < len; i++){
    retVal += dp[start + i] * to_power(16, power);
    power += 2;
  }
  return retVal;
}

/* 
 * Returns the n-th FAT entry in disk image 'dp'
 */ 
int get_nth_fat_entry(unsigned char *dp, int n){
  int b1, b2;
  int value;
  int index1 = (3*n)/2;
  int index2 = index1 + 1;
  b1 = dp[FAT_T_START + index1];
  b2 = dp[FAT_T_START + index2];
  
  if (n%2 != 0){
    b1 = b1 & 0xF0;
    value = (b2 << 4) + (b1 >> 4);
  }
  else{
    b2 = b2 & 0x0F;
    value = (b2 << 8) + b1;
  }
  return value;
}

/* 
 * Returns the number of free sectors in disk image 'dp'
 */ 
int get_free_sectors(unsigned char *dp){
  int count = 0;
  int sectors = get_int(dp, SEC_COUNT_START, SEC_COUNT_LEN) - NUM_RSRV_SECTORS;

  for (int i = 2; i <= sectors; i ++){
    if (get_nth_fat_entry(dp, i) == 0x00) count++;
  }
  return count;
}

/* 
 * Gets the volume label for disk image 'dp' from the root directory
 * and stores the string in 'retStr'
 */ 
void get_vol_label(unsigned char*dp, char *retStr){
  for (int i = ROOT_START; i < ROOT_END; i += DIR_ENTRY_LEN){
    unsigned char attr = dp[i + ATTR_OFFSET];
    if (attr != 0x0F && attr & LABEL_MASK){
      get_string(dp, retStr, i, LABEL_LEN);
      return;
    }
  }
}

/*
*  returns TRUE if the given offset refers to a file or directory
*  else, returns FALSE
*/
int is_file(unsigned char *dp, int offset){

  int isFile = TRUE;
  
  unsigned char attr = dp[offset + ATTR_OFFSET];
  unsigned char first_byt_name = dp[offset];
  int first_log_cluster = dp[offset + FIRST_LOG_OFFSET] + (dp[offset + FIRST_LOG_OFFSET +1] << 8);
    
  if (first_byt_name == 0x00) isFile = FALSE; //entry and all subsequent entries are free
  if (first_byt_name == 0xE5) isFile = FALSE; // entry is free
  if (attr == 0x0F || attr & LABEL_MASK) isFile = FALSE;  
  if (first_log_cluster == 0 || first_log_cluster == 1) isFile = FALSE;

  return isFile;
}

/*
*  Returns 'F' if it is a file, 'D' if it is a subdirectory
*/
char get_type(unsigned char *dp, int dirOffset){

  unsigned char attr = dp[dirOffset + ATTR_OFFSET];
  if (attr & SUBDIR_MASK){
    return 'D';
  }
  return 'F';
}

/*
*  Returns the index of the physical sector associated with the given FATentry
*/
int map_to_phys(int FATentry){
  return (FATentry + MAP_TO_PHYS)*SECTOR_SZ;
}

/*
*  Returns the first logical cluster of the file at entryOffset
*/ 
int get_first_log_cluster(unsigned char *dp, int entryOffset){
 return dp[entryOffset + FIRST_LOG_OFFSET] + (dp[entryOffset + FIRST_LOG_OFFSET +1] << 8);
}

/*
*  Returns the number of files and subdirectories in the given directory
*  Not recursive - doesn't count files/subdirs within subdirs
*/
int get_num_files_and_dirs(unsigned char *str, int dirOffset){
  int count = 0;
  int start, end;
  if (dirOffset == ROOT_START){
    start = ROOT_START;
    end = ROOT_END;
  }
  else{
    start = dirOffset;
    end = dirOffset + SECTOR_SZ -1;
  }
  
  for (int i = start; i < end; i += DIR_ENTRY_LEN){
    if (is_file(str, i)) count++;
  }
  return count;
}

/* 
 * Returns the total number of files on disk image 'dp' - does not include subdirectories
 */ 
int get_num_files(unsigned char *str, int dirOffset){
  int count = 0;
  int start, end;
  if (dirOffset == ROOT_START){
    start = ROOT_START;
    end = ROOT_END;
  }
  else{
    start = dirOffset;
    end = dirOffset + SECTOR_SZ -1;
  }
  for (int i = start; i < end; i += DIR_ENTRY_LEN){
    if (is_file(str, i)){ 
      if (get_type(str, i) == 'D'){
      
        if (str[i] == '.') continue;
        
        int nextOffset = map_to_phys(get_first_log_cluster(str, i));

        count += get_num_files(str, nextOffset);
      }
      else count++;
    }
  }
  return count;
}

void pad_str(char *str, int len){
  int count = 0;
  char c = str[0];
  while (count < len && c) c = str[++count];
  for (int i = count; i<len; i++){
    str[i] = 0x20;
  }
}

void write_str(unsigned char *dp, char* str, int start, int len){
  for (int i = 0; i < len; i++){
    dp[i + start] = (unsigned char)str[i];
  }
}

void write_int(unsigned char *dp, int val, int start, int len){
  for (int i = 0; i<len; i++){
    dp[start+i] = (val >> (i*8))&0xFF;
  }  
}

void write_long(unsigned char *dp, long val, int start, int len){

  for (int i = 0; i<len; i++){
    dp[start+i] = (val >> (i*8))&0xFF;
  } 
}
    
void write_dir_entry (unsigned char *dp, int first_log_entry, char *path, char *fname, char*ext, time_t dt, off_t fsize, int offset){

  int entryIndex = get_next_empty_entry(dp, offset);
  
  pad_str(fname, FNAME_LEN);
  write_str(dp, fname, entryIndex, FNAME_LEN);

  pad_str(ext, EXT_LEN);
  write_str(dp, ext, entryIndex+EXT_START, EXT_LEN);
  
  write_long(dp, fsize, entryIndex+FILE_SZ_START, FILE_SZ_LEN);
  
  write_int(dp, first_log_entry, entryIndex+FIRST_LOG_OFFSET, 2);
  
  struct tm *dt_tm;
   dt_tm = gmtime(&dt);
  
  // the following code is based on the conversion found on stackoverflow 
  // https://stackoverflow.com/questions/15763259/unix-timestamp-to-fat-timestamp
  unsigned long FatTime = ((dt_tm->tm_year - 80) << 25) | 
    ((dt_tm->tm_mon+1) << 21) |
    (dt_tm->tm_mday << 16) |
    (dt_tm->tm_hour << 11) |
    (dt_tm->tm_min << 5) |
    (dt_tm->tm_sec >> 1);
  
  write_long(dp, FatTime, entryIndex+TIME_OFFSET, 4);

}


int get_next_empty_entry(unsigned char *p, int dirOffset){
  int start, end;
  if (dirOffset == ROOT_START){
    start = ROOT_START;
    end = ROOT_END;
  }
  else{
    start = dirOffset;
    end = dirOffset + SECTOR_SZ -1;
  }
  
  for (int i = start; i < end; i += DIR_ENTRY_LEN){
    if (p[i] == 0x00 || p[i] == 0xE5){
      return i;
    }
  }  
  
  return -1;
}

int get_next_free_log_cluster(unsigned char *p){
  int start = 2;
  int sectors = get_int(p, SEC_COUNT_START, SEC_COUNT_LEN) - NUM_RSRV_SECTORS;
  for (int i = start; i< sectors; i++){
    if (get_nth_fat_entry(p, i) == 0x00) return i;
  }
  return -1;
}

int get_subdir_offset(unsigned char *dp, char *dname, int offset){
  int end = offset + SECTOR_SZ;
  if (offset == ROOT_START) end = ROOT_END;
  for (int i = offset; i<end; i+=32){
    if (get_type(dp, i) == 'D'){
      char * thisDirName = (char *)calloc(8, sizeof(char));
      get_string(dp, thisDirName, i, FNAME_LEN);
      trim_whitespace(thisDirName);
      if (!strcmp(thisDirName, dname)){
        return map_to_phys(get_first_log_cluster(dp, i));
      }
    }
  } 
  return -1;
}
 

void write_nth_fat_entry(unsigned char * dp, int n, int val){
  int b1, b2;
  int index1 = FAT_T_START + (3*n)/2;
  int index2 = index1 + 1; 
  
  if(n%2 == 0){ 
    b1 = val & 0xFF;
    dp[index1] = b1;
    
    //get the bits that need to remain
    b2 = (dp[index2] & 0xF0);
    //add new bits
    b2 |= ((val >> 8) & 0x0F);
    dp[index2] = b2;
  }
  else{ 
    b1 = (val << 4) & 0xF0;
    dp[index1] = b1;

    // get the bits that need to remain
    b2 = (dp[FAT_T_START + index1] & 0x0F);
    //add new bits
    b2 |= ((val >> 4) & 0xFF);
    dp[index2] = b2;
  }
} 
 
  
void copy_to_disk(unsigned char *p, FILE *fp, int dirOffset, off_t fsize, int current_log_cluster){
  

  // next phys sec = map_to_phys(first log cluster...
  int next_phys_sec = map_to_phys(current_log_cluster);
  
  for (int i = 0; i<fsize; i+= SECTOR_SZ){

    int sz = SECTOR_SZ;
    if (fsize-i <SECTOR_SZ) sz=fsize-i;
    
    unsigned char buffer[SECTOR_SZ];
    fread(buffer, sizeof(char), sz, fp);
    memcpy(p+next_phys_sec, buffer, sz);
    
    
    if (fsize-i <SECTOR_SZ) write_nth_fat_entry(p, current_log_cluster, 0xff);
    else{
      //temp write to this log cluster so that it gets skipped on check for next!
      write_nth_fat_entry(p, current_log_cluster, 1);
      int next_log_cluster = get_next_free_log_cluster(p);
      write_nth_fat_entry(p, current_log_cluster, next_log_cluster);
      current_log_cluster = next_log_cluster;
      next_phys_sec = map_to_phys(current_log_cluster);
    }    
  }   
}

