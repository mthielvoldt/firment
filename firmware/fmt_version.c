#include <fmt_version.h>
#include "fmt_comms.h"

bool fmt_sendVersion(void)
{
  Top msg = {
    .which_sub = Top_Version_tag,
    .sub = {.Version = {
      .major = VERSION_MAJOR,
      .minor = VERSION_MINOR,
      .patch = VERSION_PATCH,
    }}
  };
  return fmt_sendMsg(msg);
}