#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mini_valgrind.h"

#undef malloc
#undef free

/*
 * Replace normal malloc, this malloc will also create meta data info
 * and insert it to the head of the list.
 * You have to malloc enough size to hold both the size of your allocated
 * and the meta_data structure.
 *
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 *
 * @return
 *	On success, return a pointer to the memory block allocated by
 *	the function. Note that this pointer should return the actual
 *	address the pointer starts and not the meta_data.
 *
 *	If the function fails to allocate the requested block of memory,
 *	return a null pointer.
 */
void *mini_malloc(size_t size, const char *file, size_t line) {
  // your code here
  void *chunckOfMemory = (void *) malloc(size + sizeof(meta_data));
  if(chunckOfMemory == NULL)
  {
    return NULL;
  }
  meta_data * inMemory = chunckOfMemory;
  void * actAddress = chunckOfMemory + sizeof(meta_data);
  inMemory->address = actAddress;
  insert_meta_data(inMemory,size, file, line);
  total_usage +=size;
  return inMemory->address;
}

/*
 * Replace normal free, this free will also delete the node in the list.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated. If a null pointer is
 *	passed, no action occurs.
 */
void mini_free(void *ptr) {
  // your code here
  if(ptr == NULL)
  {
    return;
  }
  remove_meta_data(ptr);
}

/*
 * Helper function to insert the malloc ptr node to the list.
 * Accumulate total_usage here.
 *
 * @param md
 * 	Pointer to the meta_data
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 */
void insert_meta_data(meta_data *md, size_t size, const char *file,
                      size_t line) {
  /* set value for malloc_info*/
  if(head == NULL)
  {
    head = md;
    md->next = NULL;
  }
  else
  {
    md->next = head;
    head = md;
  }
  md->size = size;
  strncpy(md->file_name, file, MAX_FILENAME_LENGTH);
  md->line_num = line;
}

/*
 * Helper function to remove the free ptr node from the list.
 * Add to total_free here.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated.
 */
void remove_meta_data(void *ptr) {  //ptr = actual memory block
  /* check if ptr is in the list and delete it from list */
  if(head == NULL)
  {
    bad_frees++;
    return;
  }
  meta_data * prev = head;
  meta_data * thead = head;
  while(thead!= NULL && thead->address != ptr)
  {
    prev = thead;
    thead = thead->next;
  }
  if(thead == NULL)
  {
    bad_frees++; //actualAddress was not an element in the list
    return;
  }
  else
  {
    if(prev == thead) //first node in the ll
    {
      head = head->next;
      total_free += thead->size;
      free(thead);
    }
    else
    {
      prev->next = thead->next;
      total_free += thead->size;
      free(thead);
    }
  }
}

/*
 * Deletes all nodes from the list. Called when the program exits so make sure
 * to not count these blocks as freed.
 */
void destroy() {
  // your code here
  if(head == NULL)
  {
    return;
  }
  meta_data * n = NULL;
  while(head != NULL)
  {
    n = head->next;
    free(head);
    head = n;
  }
  head = NULL;
}

/*
 * Print mini_valgrind leak report. DO NOT TOUCH THIS. You risk failing the
 * autograder if you do.
 */

void print_report() {
  meta_data *leak_info;

  FILE *fp_write = fopen("result.txt", "wt");

  size_t total_leak = 0;

  if (fp_write != NULL) {
    fprintf(fp_write, "Heap report:\n\n");
    leak_info = head;
    while (leak_info != NULL) {
      total_leak += leak_info->size;
      fprintf(fp_write, "\tLeak file %s : line %zu\n", leak_info->file_name,
              leak_info->line_num);
      fprintf(fp_write, "\tLeak size : %zu bytes\n", leak_info->size);
      fprintf(fp_write, "\tLeak memory address : %p\n", leak_info->address);
      fputs("\n", fp_write);
      leak_info = leak_info->next;
    }
    fprintf(fp_write, "\nProgram made %zu bad call(s) to free.\n", bad_frees);
    fprintf(fp_write, "\nHeap summary:\n\n");
    fprintf(fp_write,
            "\tTotal memory usage: %zu bytes, Total memory free: %zu bytes\n",
            total_usage, total_free);
    if (total_leak != 0) {
      fprintf(fp_write, "\tTotal leak: %zu bytes\n", total_leak);
    } else {
      fprintf(fp_write, "\tNo leak, all memory are freed, Congratulations!\n");
    }
    fclose(fp_write);
  }
  destroy();
}
