#ifndef BPS_H
#define BPS_H

#include "file_ex.h"

#define BPS_MAGIC ("BPS1")

typedef struct BPS_State_t {
  SizedPtr_t patch;
  SizedPtr_t source;
  SizedPtr_t target;

  uint64_t outputOffset;
  uint64_t patchOffset;
  int64_t sourceRelativeOffset;
  int64_t targetRelativeOffset;
} BPS_State_t;

typedef enum BPS_ActionType_t {
  BPS_ActionType_SourceRead,
  BPS_ActionType_TargetRead,
  BPS_ActionType_SourceCopy,
  BPS_ActionType_TargetCopy,
} BPS_ActionType_t;

typedef struct BPS_Action_t {
  BPS_ActionType_t type;
  uint64_t length;
} BPS_Action_t;

bool BPS_ReadMagic(BPS_State_t* state);
bool BPS_ReadUnsigned(BPS_State_t* state, uint64_t* value);
bool BPS_ReadAction(BPS_State_t* state, BPS_Action_t* action);

bool BPS_SourceRead(BPS_State_t* state, uint64_t length);
bool BPS_TargetRead(BPS_State_t* state, uint64_t length);
bool BPS_SourceCopy(BPS_State_t* state, uint64_t length);
bool BPS_TargetCopy(BPS_State_t* state, uint64_t length);

#endif // BPS_H
