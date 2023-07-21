// the elements corrected are already referenced in. other files 
#include "common.h"

#include "preload.h"

#include "log.h"

#include "cmdline.h"

#include "conf.h"

#include "state.h"

#include <signal.h>

#include <grp.h>

/* variables */

const char *conffile = DEFAULT_CONFFILE;

const char *statefile = DEFAULT_STATEFILE;

const char *logfile = DEFAULT_LOGFILE;

int nicelevel = DEFAULT_NICELEVEL;

int foreground = 0;

/* local variables */

int

main (int argc, char **argv)

{

/* initialize */

preload_cmdline_parse (&argc, &argv);

preload_log_init (logfile);

preload_conf_load (conffile, TRUE);

set_sig_handlers ();

if (!foreground)

daemonize ();

if (0 > nice (nicelevel))

g_warning ("%s", strerror (errno));

g_debug ("starting up");

preload_state_load (statefile);

/* main loop */

GMainLoop *main_loop = g_main_loop_new (NULL, FALSE);

preload_state_run (statefile);

g_main_loop_run (main_loop);

/* clean up */

preload_state_save (statefile);

if (preload_is_debugging ())

preload_state_free ();

g_debug ("exiting");

return EXIT_SUCCESS;

}
