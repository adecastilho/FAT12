/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  DirList.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Directories{
  int FAT;
  int index;
  char path [100];
  struct Directories *next;
} Dirs;

void remove_dir(Dirs **head);
void add_dir(Dirs **head, int FAT, int index, char path[]);
int length(Dirs **head);
