#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../wavheader.h"

int main() {
  // filename
  char filename[] = "../audio/sd015.wav";
  // open file
  FILE *file = fopen(filename, "rb");
  if (!file) {
    printf("Error: file not found\n");
    return 1;
  }
  // read header
  WavHeader header;
  fread(&header, sizeof(WavHeader), 1, file);
  // close file
  fclose(file);
  // print header
  WavHeader_print(&header);
  return 0;
}
