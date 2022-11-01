#ifndef FILE_EX_H
#define FILE_EX_H

#include "common.h"

typedef struct File_t {
  FILE* ptr;
  size_t size;
} File_t;

typedef struct SizedPtr_t {
  uint8_t* data;
  size_t size;
} SizedPtr_t;

size_t GetFileSize(FILE* fp);
File_t Open(const char *__restrict__ __filename, const char *__restrict__ __modes);
SizedPtr_t ReadToEnd(const char *__restrict__ __filename, const char *__restrict__ __modes);

#endif
