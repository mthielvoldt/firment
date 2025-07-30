#ifndef assert_H
#define assert_H

#define ASSERT_ARM_OK(x)  \
  if (x != ARM_DRIVER_OK) \
  return false
#define ASSERT_SUCCESS(x) \
  if (!x)                 \
  return false

#endif
