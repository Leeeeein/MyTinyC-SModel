#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>
#include<stdlib.h>
#include<unistd.h>

void* dalloc(size_t num_elements, size_t elements_size)
{
  void* ptr = malloc(num_elements * elements_size);
  return ptr;
}
int main(int argc,char *argv[])
{
  int* p = (int*)dalloc(10, sizeof(int));
  for(int i = 0; i < 10; i++)
  {
    p[i] = i; 
  }
  for(int i = 0; i < 10; i++)
  {
    printf("%d ", *(p+i));
  }
  return 0;
}
