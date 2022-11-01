#include "file_ex.h"
#include "crc.h"
#include "bps.h"
#include "stdio.h"

#define PATCH_FILE  "patches/SUPERSTAR.bps"
#define ROM_FILE    "roms/Super Mario World (USA).smc"
#define TARGET_FILE "roms/patched.smc"

int main() {
  BPS_State_t state = {0};
  printf("Reading source and patch file...\n");
  state.patch = ReadToEnd(PATCH_FILE, "rb");
  state.source = ReadToEnd(ROM_FILE, "rb");

  printf("Read patch file (%ld bytes)\n", state.patch.size);
  printf("Read source file (%ld bytes)\n", state.source.size);

  printf("Reading patch header...\n");
  if (!BPS_ReadMagic(&state)) {
    printf("Invalid magic in patch file, exiting.\n");
    return 1;
  }

  uint64_t sourceSize = 0;
  if (!BPS_ReadUnsigned(&state, &sourceSize)) {
    printf("Couldn't read source size.\n");
    return 1;
  }

  if (!BPS_ReadUnsigned(&state, &state.target.size)) {
    printf("Couldn't read target size.\n");
    return 1;
  }

  printf("Source size according to patch = %ld bytes\n", sourceSize);
  printf("Target size according to patch = %ld bytes\n", state.target.size);

  printf("Checking source and patch CRCs...\n");
  uint32_t sourceCRC = *(uint32_t*)(state.patch.data + (state.patch.size - 12));
  if (sourceCRC != CRC32_Digest(state.source.data, state.source.size)) {
    printf("Source CRC did not match.\n");
    return 1;
  }
  printf("Source CRC check passed.\n");

  uint32_t patchCRC = *(uint32_t*)(state.patch.data + (state.patch.size - 4));
  if (patchCRC != CRC32_Digest(state.patch.data, state.patch.size - 4)) {
    printf("Patch CRC did not match.\n");
    return 1;
  }
  printf("Patch CRC check passed.\n");

  printf("Allocating target buffer...\n");
  state.target.data = malloc(state.target.size);
  if (state.target.data == NULL) {
    printf("Allocating target buffer failed.\n");
    return 1;
  }

  uint64_t metadataSize = 0;
  if (!BPS_ReadUnsigned(&state, &metadataSize)) {
    printf("Allocating target buffer failed.\n");
    return 1;
  }
  printf("Metadata size = %ld bytes\n", metadataSize);

  state.patchOffset += metadataSize;

  printf("Applying patch actions...\n");
  BPS_Action_t action;
  size_t actionsTaken = 0;
  while (state.patchOffset < state.patch.size - 12) {
    if (!BPS_ReadAction(&state, &action)) {
      printf("Failed to read action.\n");
      return 1;
    }

    switch (action.type) {
      case BPS_ActionType_SourceRead: {
        if (!BPS_SourceRead(&state, action.length)) {
          printf("SourceRead failed\n");
          return 1;
        }
      } break;

      case BPS_ActionType_TargetRead: {
        if (!BPS_TargetRead(&state, action.length)) {
          printf("TargetRead failed\n");
          return 1;
        }
      } break;

      case BPS_ActionType_SourceCopy: {
        if (!BPS_SourceCopy(&state, action.length)) {
          printf("SourceCopy failed\n");
          return 1;
        }
      } break;

      case BPS_ActionType_TargetCopy: {
        if (!BPS_TargetCopy(&state, action.length)) {
          printf("TargetCopy failed\n");
          return 1;
        }
      } break;
    }

    actionsTaken++;
  }
  printf("Finished applying %ld patch actions.\n", actionsTaken);

  printf("Checking target CRC...\n");
  uint32_t targetCRC = *(uint32_t*)(state.patch.data + (state.patch.size - 8));
  if (targetCRC != CRC32_Digest(state.target.data, state.target.size)) {
    printf("Target CRC did not match.\n");
    return 1;
  }
  printf("Target CRC check passed.\n");

  printf("Writing target to file %s\n", TARGET_FILE);
  FILE* f = fopen(TARGET_FILE, "wb");
  fwrite(state.target.data, state.target.size, 1, f);

  // Cleanup
  printf("Done. Cleaning up.\n");
  fclose(f);
  free(state.target.data);
  free(state.patch.data);
  free(state.source.data);

  return 0;
}
