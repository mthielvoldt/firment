#ifndef fmt_assert_H
#define fmt_assert_H

#include <cmsis_gcc.h>

// These can be customized to facilitate init debugging.

#define ASSERT_ARM_OK(x)  \
  if (x != ARM_DRIVER_OK) \
  __BKPT(0)
  
#define ASSERT_SUCCESS(x) \
  if (!x)                 \
  __BKPT(1)

#define ASSERT_EQUAL(x, y) \
  if (x != y)              \
  __BKPT(2)

#endif
