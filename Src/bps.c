#include "bps.h"

bool BPS_ReadMagic(BPS_State_t* state) {
  if (state->patchOffset + 4 > state->patch.size) return false;
  bool returnValue = strncmp((char*)(state->patch.data + state->patchOffset), BPS_MAGIC, 4) == 0;
  state->patchOffset += 4;
  return returnValue;
}

bool BPS_ReadUnsigned(BPS_State_t* state, uint64_t* value) {
  uint64_t data = 0;
  uint8_t byte = 0;
  uint64_t shift = 1;

  while (true) {
    if (state->patchOffset + 1 > state->patch.size) return false;
    byte = state->patch.data[state->patchOffset++];

    data += ((byte & 0x7f) * shift);
    if (byte & 0x80) break;

    shift <<= 7;
    data += shift;
  }

  *value = data;
  return true;
}

bool BPS_ReadAction(BPS_State_t* state, BPS_Action_t* action) {
  uint64_t value = 0;
  if (!BPS_ReadUnsigned(state, &value)) return false;

  action->type = value & 0x3;
  action->length = (value >> 2) + 1;

  return true;
}

bool BPS_SourceRead(BPS_State_t* state, uint64_t length) {
    if (state->outputOffset + length > state->source.size) return false;
    if (state->outputOffset + length > state->target.size) return false;

  while (length-- > 0) {
    state->target.data[state->outputOffset] = state->source.data[state->outputOffset];
    state->outputOffset++;
  }

  return true;
}

bool BPS_TargetRead(BPS_State_t* state, uint64_t length) {
  if (state->patchOffset + length > state->patch.size) return false;
  if (state->outputOffset + length > state->target.size) return false;

  while (length-- > 0) {
    state->target.data[state->outputOffset++] = state->patch.data[state->patchOffset++];
  }

  return true;
}

bool BPS_SourceCopy(BPS_State_t* state, uint64_t length) {
  uint64_t data = 0;
  if (!BPS_ReadUnsigned(state, &data)) return false;

  state->sourceRelativeOffset += (data & 1 ? -1 : 1) * (data >> 1);

  if (state->sourceRelativeOffset < 0) return false;
  if (state->sourceRelativeOffset + length > state->source.size) return false;
  if (state->outputOffset + length > state->target.size) return false;

  while (length-- > 0) {
    state->target.data[state->outputOffset++] = state->source.data[state->sourceRelativeOffset++];
  }

  return true;
}

bool BPS_TargetCopy(BPS_State_t* state, uint64_t length) {
  uint64_t data = 0;
  if (!BPS_ReadUnsigned(state, &data)) return false;

  state->targetRelativeOffset += (data & 1 ? -1 : 1) * (data >> 1);

  if (state->targetRelativeOffset < 0) return false;
  if (state->targetRelativeOffset + length > state->target.size) return false;
  if (state->outputOffset + length > state->target.size) return false;

  while (length-- > 0) {
    state->target.data[state->outputOffset++] = state->target.data[state->targetRelativeOffset++];
  }

  return true;
}
