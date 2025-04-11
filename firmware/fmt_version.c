#include <fmt_version.h>
#include "fmt_comms.h"

static uint32_t (*buildIdGetter)(void) = NULL;

void fmt_setBuildIdGetter(uint32_t (*getter)(void))
{
  buildIdGetter = getter;
}

bool fmt_sendVersion(void)
{
  Top msg = {
      .which_sub = Top_Version_tag,
      .sub = {
          .Version = {
              .major = VERSION_MAJOR,
              .minor = VERSION_MINOR,
              .patch = VERSION_PATCH,
          },
      },
  };
  if (buildIdGetter)
    msg.sub.Version.buildId = buildIdGetter();

  return fmt_sendMsg(msg);
}