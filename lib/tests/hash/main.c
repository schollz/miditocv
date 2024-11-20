
#include <assert.h>
#include <stdio.h>

#include "../../hash.h"

int main() {
  printf("hash_djb(\"helloworld\") = %u\n", hash_djb("helloworld"));
  assert(hash_djb("helloworld") == 4294815105);
  printf("hash_djb(\"hello\") = %u\n", hash_djb("hello"));
  assert(hash_djb("hello") == 261238937);
  printf("hash_djb(\"world\") = %u\n", hash_djb("world"));
  assert(hash_djb("world") == 279393645);
  return 0;
}