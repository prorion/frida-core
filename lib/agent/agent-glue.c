#include "myagnt.h"

#include "frida-base.h"
#include "frida-payload.h"

#ifdef HAVE_ANDROID
# include <jni.h>
# if __ANDROID_API__ < __ANDROID_API_L__
#  include <signal.h>
# endif
#endif
#ifdef HAVE_GIOOPENSSL
# include <gioopenssl.h>
#endif

#ifdef _WIN32
# define MYAGNT_EXPORT __declspec(dllexport)
#else
# define MYAGNT_EXPORT __attribute__((visibility("default")))
#endif

static FridaUnloadPolicy unload_policy = FRIDA_UNLOAD_POLICY_IMMEDIATE;
static void * injector_state = NULL;

extern void myagnt_agent_main (const gchar * agent_parameters, FridaUnloadPolicy * unload_policy, void * injector_state);

void
myagnt_init_memory (void)
{
#ifdef HAVE_MUSL
  static gboolean been_here = FALSE;

  if (been_here)
    return;
  been_here = TRUE;
#endif

  gum_init_embedded ();
  gio_init ();

  g_thread_set_garbage_handler (_frida_agent_myagnt_agent_on_pending_thread_garbage, NULL);

#ifdef HAVE_GIOOPENSSL
  g_io_module_openssl_register ();
#endif

  gum_script_backend_get_type (); /* Warm up */
  frida_error_quark (); /* Initialize early so GDBus will pick it up */

#if defined (HAVE_ANDROID) && __ANDROID_API__ < __ANDROID_API_L__
  /*
   * We might be holding the dynamic linker's lock, so force-initialize
   * our bsd_signal() wrapper on this thread.
   */
  bsd_signal (G_MAXINT32, SIG_DFL);
#endif

  gum_internal_heap_ref ();
}

void
myagnt_deinit_memory (void)
{
#ifndef HAVE_MUSL
  gum_shutdown ();
  gio_shutdown ();
  glib_shutdown ();

  gio_deinit ();
  gum_deinit_embedded ();

  frida_run_atexit_handlers ();

# ifdef HAVE_DARWIN
  /* Do what myagnt_deinit_memory() does on the other platforms. */
  gum_internal_heap_unref ();
# endif
#endif
}

#ifdef HAVE_ANDROID

jint
JNI_OnLoad (JavaVM * vm, void * reserved)
{
  FridaAgentBridgeState * state = reserved;

  myagnt_agent_main (state->agent_parameters, &state->unload_policy, state->injector_state);

  return JNI_VERSION_1_6;
}

#endif

MYAGNT_EXPORT void
myagnt_main (const char * data)
{
  myagnt_agent_main (data, &unload_policy, injector_state);
}
