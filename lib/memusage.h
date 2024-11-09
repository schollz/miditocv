#include <malloc.h>

uint32_t getTotalHeap() {
  extern char __StackLimit, __bss_end__;

  return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap() {
  struct mallinfo m = mallinfo();

  return getTotalHeap() - m.uordblks;
}

void print_memory_usage() {
  uint32_t total_heap = getTotalHeap();
  uint32_t used_heap = total_heap - getFreeHeap();
  printf("memory usage: %2.1f%% (%ld/%ld)\n",
         (float)(used_heap) / (float)(total_heap)*100.0, used_heap, total_heap);
}