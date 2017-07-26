#ifndef MOTION_H
#define MOTION_H

#include <Sm_Pluse.h>

#define AXIS_QTY 6 //最大轴数 

typedef enum {
    VelIns = 0,
    AbsIns = 1,
    RelIns = 2,
} InstructionId;

typedef struct {
    InstructionId Ins;
    uint8_t Axis;
    int64_t Argument;
} MotionIns_t;

int Sm_SetVelmode(uint8_t axis);
int Sm_SetPosmode(uint8_t axis);
int Sm_SetVelmodePrm(uint8_t axis, float acc, float dec, uint32_t startUp = SM_STARTUP_SPEED);
int Sm_SetPosmodePrm(uint8_t axis, float acc, float dec, uint32_t startUp = SM_STARTUP_SPEED);
int32_t Sm_GetVel(uint8_t axis);
int Sm_SetVel(uint8_t axis, int32_t vel);
int64_t Sm_GetPos(uint8_t axis);
int Sm_SetAbsolutePos(uint8_t axis, int64_t absPos);
int Sm_SetRelativePos(uint8_t axis, int64_t relPos);

#endif
