/* preload.c - preload daemon body
 *
 * Copyright (C) 2005  Behdad Esfahbod
 * Portions Copyright (C) 2000  Andrew Henroid
 * Portions Copyright (C) 2001  Sun Microsystems (thockin@sun.com)
 *
 * This file is part of preload.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

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

static GMainLoop *main_loop;



/* Functions */

static void
daemonize (void)
{
  switch (fork ())
    {
    case -1:
      g_error ("fork failed, exiting: %s", strerror (errno));
      exit (EXIT_FAILURE);
      break;
    case 0:
      /* child */
      break;
    default:
      /* parent */
      if (getpid () == 1)
        {
          /* chain to /sbin/init if we are called as init! */
          execl ("/sbin/init", "init", NULL);
          execl ("/bin/init", "init", NULL);
        }
      exit (EXIT_SUCCESS);
    }

  /* disconnect */
  setsid ();
  umask (0117);

  /* get outta the way */
  (void) chdir ("/");
}


/* signal handling */


static gboolean
sig_handler_sync (gpointer data)
{
  switch (GPOINTER_TO_INT (data)) {
    case SIGHUP:
      preload_conf_load (conffile, FALSE);
      preload_log_reopen (logfile);
      break;
    case SIGUSR1:
      preload_state_dump_log ();
      preload_conf_dump_log ();
      break;
    case SIGUSR2:
      preload_state_save (statefile);
      break;
    default: /* everything else is an exit request */
      g_message ("exit requested");
      g_main_loop_quit (main_loop);
      break;
  }
  return FALSE;
}

static RETSIGTYPE
sig_handler (int sig)
{
  g_timeout_add (0, sig_handler_sync, GINT_TO_POINTER (sig));
}

static void
set_sig_handlers (void)
{
  /* trap key signals */
  signal (SIGINT,  sig_handler);
  signal (SIGQUIT, sig_handler);
  signal (SIGTERM, sig_handler);
  signal (SIGHUP,  sig_handler);
  signal (SIGUSR1, sig_handler);
  signal (SIGUSR2, sig_handler);
  signal (SIGPIPE, SIG_IGN);
}


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
  main_loop = g_main_loop_new (NULL, FALSE);
  preload_state_run (statefile);
  / Check if the resource already exists in the preload queue.
for (int i = 0; i < preload_queue_len; i++) {
  if (strcmp(preload_queue[i].path, path) == 0) {
    // The resource already exists, so do nothing.
    return;
  }
}

// The resource does not exist, so add it to the queue.
preload_queue[preload_queue_len].path = path;
preload_queue_len++;
  g_main_loop_run (main_loop);

  /* clean up */
  preload_state_save (statefile);
  if (preload_is_debugging ())
    preload_state_free ();
  g_debug ("exiting");
  return EXIT_SUCCESS;
}
