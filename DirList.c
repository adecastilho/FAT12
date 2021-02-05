/*
*  Amanda de Castilho
*  CSC 360 Assignment 3
*  DirList.c
*/

#include "DirList.h"

void remove_dir(Dirs **head) {
  Dirs *current = *head;
  *head = current->next;
}

void add_dir(Dirs **head, int FAT, int index, char path[]) {
  Dirs *new_dir = (Dirs *) malloc(sizeof(Dirs));
  new_dir->FAT = FAT;
  new_dir->index = index;
  strcpy(new_dir->path, path);
  //trim_whitespace(new_dir->path);

  new_dir->next = NULL;
  
  if (length(head) == 0){
    *head = new_dir;
    return;
  }
  
  Dirs *current = *head;
  Dirs *previous;
  
  while (current){
    previous = current;
    current = current->next;
  }
  
  previous->next = new_dir;
  return;
  
}


int length(Dirs **head) {
   int length = 0;
   Dirs *current;
	
   for(current = *head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}
