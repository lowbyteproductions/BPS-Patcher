#include "file_ex.h"

size_t GetFileSize(FILE* fp) {
  if (fseek(fp, 0, SEEK_END) < 0) {
    return 0;
  }

  int64_t fileSize = ftell(fp);
  if (fileSize < 0) {
    fileSize = 0;
  }
  rewind(fp);

  return (size_t)fileSize;
}

File_t Open(const char* restrict filename, const char* restrict modes) {
  File_t f;
  f.ptr = fopen(filename, modes);
  f.size = 0;

  if (f.ptr != NULL) {
    f.size = GetFileSize(f.ptr);
  }

  return f;
}

SizedPtr_t ReadToEnd(const char *restrict filename, const char *__restrict__ modes) {
  SizedPtr_t fb;
  File_t ft = Open(filename, modes);

  if (ft.ptr == NULL || ft.size == 0) {
    fb.data = NULL;
    fb.size = 0;
    return fb;
  }

  fb.size = ft.size;
  fb.data = malloc(fb.size);
  fread(fb.data, fb.size, 1, ft.ptr);
  fclose(ft.ptr);

  return fb;
}
