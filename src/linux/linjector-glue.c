#include "frida-core.h"

GBytes *
_myagnt_descriptor_clone_so (GBytes * so)
{
  /* TODO: update .so identity */
  return g_bytes_ref (so);
}
