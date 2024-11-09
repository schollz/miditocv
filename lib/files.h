#ifndef FILES_H
#define FILES_H

#include "wavheader.h"

typedef struct File {
  char *filename;
  FSIZE_t size;
  uint32_t sample_pos;
  uint8_t is_stereo;
  bool is_valid_audio;
} File;

typedef struct Files {
  File *files;
  uint32_t num_files;
} Files;

void File_free(File *f) {
  if (f != NULL) {
    free(f->filename);
  }
}

#define WAV_HEADER_SIZE 44

void Wav_read(File *f, int16_t *data, uint32_t samples) {
  FIL fil;
  FRESULT fr;
  if (f->is_stereo) {
    samples = samples * 2;
  }
  fr = f_open(&fil, f->filename, FA_READ);
  if (fr != FR_OK) {
    printf("[File_read] f_open %s\n", FRESULT_str(fr));
    return;
  }
  fr = f_lseek(&fil, WAV_HEADER_SIZE + (f->sample_pos * sizeof(int16_t)));
  if (fr != FR_OK) {
    printf("[File_read] f_lseek %s\n", FRESULT_str(fr));
    f_close(&fil);  // Don't forget to close the file
    f->sample_pos = 0;
    return;
  }
  unsigned int bytes_read;
  fr = f_read(&fil, data, samples * sizeof(int16_t), &bytes_read);
  if (fr != FR_OK) {
    printf("[File_read] f_read %s\n", FRESULT_str(fr));
    f_close(&fil);  // Don't forget to close the file
    f->sample_pos = 0;
    return;
  }
  //   printf("f_read %d bytes\n", bytes_read);
  if (bytes_read >= f->size) {
    // if the file is smaller than the buffer, just reset the position
    f_close(&fil);  // Don't forget to close the file
    f->sample_pos = 0;
    return;
  } else if ((bytes_read / sizeof(int16_t)) < samples) {
    // read the rest from the beginning
    fr = f_lseek(&fil, WAV_HEADER_SIZE);
    if (fr != FR_OK) {
      printf("[File_read] f_lseek(2) %s\n", FRESULT_str(fr));
      f_close(&fil);  // Don't forget to close the file
      f->sample_pos = 0;
      return;
    }
    unsigned int bytes_read2;
    fr = f_read(&fil, data + (bytes_read / sizeof(int16_t)),
                samples * sizeof(int16_t) - bytes_read, &bytes_read2);
    if (fr != FR_OK) {
      printf("[File_read] f_read(2) %s\n", FRESULT_str(fr));
      f_close(&fil);  // Don't forget to close the file
      f->sample_pos = 0;
      return;
    }
    // printf("f_read(2) %d bytes\n", bytes_read2 + bytes_read);
    f->sample_pos = (bytes_read2 / sizeof(int16_t));
  } else {
    f->sample_pos += (bytes_read / sizeof(int16_t));
    f_close(&fil);  // Don't forget to close the file
  }
  // printf("[File_read] read %d/%d samples\n", f->sample_pos,
  //        (f->size - WAV_HEADER_SIZE) / sizeof(int16_t));
}

void File_check_wav(File *f) {
  if (f->is_valid_audio) {
    return;
  }
  FIL fil;
  FRESULT fr;
  fr = f_open(&fil, f->filename, FA_READ);
  if (fr != FR_OK) {
    printf("[File_check_wav] %s\n", FRESULT_str(fr));
    return;
  }
  WavHeader header;
  unsigned int bytes_read;
  fr = f_read(&fil, &header, sizeof(WavHeader), &bytes_read);
  if (fr != FR_OK) {
    printf("[File_check_wav] %s\n", FRESULT_str(fr));
    f_close(&fil);  // Don't forget to close the file
    return;
  }
  if (bytes_read != sizeof(WavHeader)) {
    printf("[File_check_wav] bytes_read: %d\n", bytes_read);
    f_close(&fil);  // Don't forget to close the file
    return;
  }
  // WavHeader_print(&header);
  if (header.bitsPerSample == 16) {
    f->is_valid_audio = true;
    f->is_stereo = header.NumOfChan > 1;
  }
  f_close(&fil);  // Don't forget to close the file
}

void Files_free(Files *fs) {
  if (fs == NULL) {
    return;
  }
  for (uint32_t i = 0; i < fs->num_files; i++) {
    File_free(&fs->files[i]);
  }
  free(fs->files);
  free(fs);
}

void Files_print(Files *fs) {
  for (uint32_t i = 0; i < fs->num_files; i++) {
    printf("%d %s (%llu) (%d)\n", i, fs->files[i].filename, fs->files[i].size,
           fs->files[i].is_valid_audio);
  }
}

Files *Files_malloc() {
  Files *fs = malloc(sizeof(Files));
  fs->num_files = 0;
  fs->files = NULL;
  DIR dj;      /* Directory object */
  FILINFO fno; /* File information */
  FRESULT fr;  /* File result error */
  memset(&dj, 0, sizeof dj);
  memset(&fno, 0, sizeof fno);
  fr = f_findfirst(&dj, &fno, "", "*");
  if (FR_OK != fr) {
    printf("[load_files]: f_findfirst %s\n", FRESULT_str(fr));
    return fs;
  }
  while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
    fr = f_findnext(&dj, &fno);         /* Search for next item */
    if (FR_OK != fr) {
      printf("[load_files]: f_findnext %s\n", FRESULT_str(fr));
      continue;
    }
    if (fno.fname[0] == 0) {
      continue;
    }
    printf("[Files_malloc] %s (%llu) (%d)\n", fno.fname, fno.fsize);
    fs->num_files++;
    fs->files = realloc(fs->files, fs->num_files * sizeof(File));
    fs->files[fs->num_files - 1].filename = strdup(fno.fname);
    fs->files[fs->num_files - 1].size = fno.fsize;
    fs->files[fs->num_files - 1].is_valid_audio = false;
    fs->files[fs->num_files - 1].sample_pos = 0;
  }
  f_closedir(&dj);

  if (fs->num_files == 0) {
    return fs;
  }

  for (uint32_t i = 0; i < fs->num_files; i++) {
    File_check_wav(&fs->files[i]);
  }
  // now reallocate the files array to remove invalid files
  uint32_t num_valid_files = 0;
  for (uint32_t i = 0; i < fs->num_files; i++) {
    if (fs->files[i].is_valid_audio) {
      num_valid_files++;
    }
  }
  File *valid_files = malloc(num_valid_files * sizeof(File));
  uint32_t j = 0;
  for (uint32_t i = 0; i < fs->num_files; i++) {
    if (fs->files[i].is_valid_audio) {
      valid_files[j] = fs->files[i];
      j++;
    } else {
      File_free(&fs->files[i]);  // Free invalid files
    }
  }
  free(fs->files);  // Free the old files array
  fs->files = valid_files;
  fs->num_files = num_valid_files;
  return fs;
}

#endif
