/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  helpers.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <time.h>
#include <stdint.h>
#include <arpa/inet.h>

#define NUM_LABELS 7
#define BUFFER 32
#define SECTOR_SZ 512
#define NUM_RSRV_SECTORS 32
#define OS_NAME_START 3
#define OS_NAME_LEN 8
#define TOT_SZ_START 19
#define TOT_SZ_LEN 2
#define NUM_FAT_START 16
#define NUM_FAT_LEN 1
#define SEC_COUNT_START 19
#define SEC_COUNT_LEN 2
#define SPFAT_START 22
#define SPFAT_LEN 2
#define FAT_T_START 1*SECTOR_SZ
#define FAT_T_END 10*SECTOR_SZ-1
#define ROOT_START 19*SECTOR_SZ
#define ROOT_END 33*SECTOR_SZ -1
#define DIR_ENTRY_LEN 32
#define ATTR_OFFSET 11
#define LABEL_LEN 11 
#define LABEL_MASK 0x08
#define SUBDIR_MASK 0x10
#define FIRST_LOG_OFFSET 26
#define EXT_START 8
#define EXT_LEN 3
#define FNAME_LEN 8
#define FILE_SZ_START 28
#define FILE_SZ_LEN 4
#define TIME_OFFSET 14 //offset of creation time in directory entry
#define DATE_OFFSET 16 //offset of creation date in directory entry
#define MAP_TO_PHYS 31

#define DIRSTR "%c %10d %20s %s\n"

#define TRUE 1
#define FALSE 0


typedef enum {DISKINFO, DISKLIST, DISKGET, DISKPUT} Program;


int get_len(char *str);
void extract_fname_and_ext(char *str, char *fname, char *ext);
int get_file_offset(unsigned char *dp, char *fname, char *ext);
void copy_file(unsigned char *dp, int offset, char *path);
int map_to_phys(int FATentry);
int get_first_log_cluster(unsigned char *dp, int entryOffset);
char get_type(unsigned char *dp, int dirOffset);
void trim_whitespace(char str[]);
void get_date_time(unsigned char * directory_entry_startPos, char * dt);
int check_args(Program prog, int argc);
void get_string(unsigned char *dp, char* retStr, int start, int len);
int to_power(int base, int exp);
int get_int(unsigned char *dp, int start, int len);
int get_nth_fat_entry(unsigned char *dp, int n);
int get_free_sectors(unsigned char *dp);
void get_vol_label(unsigned char *dp, char *retStr);
int get_num_files_and_dirs(unsigned char *str, int dirOffset);
int get_num_files(unsigned char *dp, int dirOffset);
int is_file(unsigned char *dp, int offset);
int get_next_empty_entry(unsigned char *p, int dirOffset);
void pad_str(char *str, int len);
void write_str(unsigned char *dp, char* str, int start, int len);
void write_int(unsigned char *dp, int val, int start, int len);
void write_long(unsigned char *dp, long val, int start, int len);
void write_dir_entry (unsigned char *dp, int first_log_entry, char *path, char *fname, char*ext, time_t dt, off_t fsize, int offset);
int get_next_free_log_cluster(unsigned char *p);
void write_nth_fat_entry(unsigned char * dp, int n, int val);
int get_subdir_offset(unsigned char *dp, char *dname, int offset);
void copy_to_disk(unsigned char *p, FILE *fp, int dirOffset, off_t fsize, int current_log_cluster);
