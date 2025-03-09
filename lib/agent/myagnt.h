#ifndef __FRIDA_MYAGNT_H__
#define __FRIDA_MYAGNT_H__

#include <glib.h>
#include "frida-base.h"

#ifdef _WIN32
# define MYAGNT_EXPORT __declspec(dllexport)
#else
# define MYAGNT_EXPORT extern __attribute__((visibility("default")))
#endif

G_BEGIN_DECLS

MYAGNT_EXPORT void myagnt_init_memory (void);
MYAGNT_EXPORT void myagnt_deinit_memory (void);
MYAGNT_EXPORT void myagnt_main (const char * data);

G_END_DECLS

#endif /* __FRIDA_MYAGNT_H__ */ 