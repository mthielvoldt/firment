#ifndef deviceId_port_H
#define deviceId_port_H

#include <stdint.h>

typedef struct {
  uint64_t dblWords[2];
} deviceId_t;

deviceId_t getDeviceId(void);

#endif