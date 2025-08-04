#ifndef fmt_assert_H
#define fmt_assert_H

#define ASSERT_ARM_OK(x)  \
  if (x != ARM_DRIVER_OK) \
  return false
#define ASSERT_SUCCESS(x) \
  if (!x)                 \
  return false

#endif
