/* conf.c - preload configuration handling routines
 *
 * Copyright (C) 2005  Behdad Esfahbod
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
#include "log.h"
#include "conf.h"
#include "state.h"

#include <math.h>
#include <time.h>

preload_conf_t conf[1];

static void
set_default_conf (preload_conf_t *conf)
{
#define true TRUE
#define false FALSE
#define default_integer(def, unit) (unit * def)
#define default_boolean(def, unit) def
#define default_enum(def, unit) def
#define default_string_list(def, unit) NULL
#define confkey(grp, type, key, def, unit) \
      conf->grp.key = default_##type (def, unit);
#include "confkeys.h"
#undef confkey
}

void
preload_conf_load (const char *conffile, gboolean fail)
{
  GKeyFile *f;
  GError *e = NULL;
  preload_conf_t newconf;
  GLogLevelFlags flags = fail ? G_LOG_LEVEL_ERROR : G_LOG_LEVEL_CRITICAL;


  set_default_conf (&newconf);
  if (conffile && *conffile)
    {
      preload_conf_t dummyconf;
      g_message ("loading conf from %s", conffile);
      f = g_key_file_new ();
      if (!g_key_file_load_from_file (f, conffile, G_KEY_FILE_NONE, &e)) {
	g_log (G_LOG_DOMAIN, flags, "failed loading conf from %s: %s", conffile, e->message);
	return;
      }

#define get_integer(grp, key, unit) (unit * g_key_file_get_integer (f, grp, key, &e))
#define get_enum(grp, key, unit) (g_key_file_get_integer (f, grp, key, &e))
#define get_boolean(grp, key, unit) g_key_file_get_boolean (f, grp, key, &e)
#define get_string_list(grp, key, unit) g_key_file_get_string_list (f, grp, key, NULL, &e)
#define confkey(grp, type, key, def, unit) \
	dummyconf.grp.key = get_##type (STRINGIZE(grp), STRINGIZE(key), unit); \
	if (!e) \
	  newconf.grp.key = dummyconf.grp.key; \
	else if (e->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND) { \
	  g_log (G_LOG_DOMAIN, flags, "failed loading conf key %s.%s: %s", \
		 STRINGIZE(grp), STRINGIZE(key), e->message); \
	  g_error_free (e); \
	  return; \
	} else { \
	  g_error_free (e); \
	  e = NULL; \
	}
#include "confkeys.h"
#undef confkey

      g_key_file_free (f);
      g_debug ("loading conf done");
    }

  /* free the old configuration */
  g_strfreev (conf->system.mapprefix);
  g_strfreev (conf->system.exeprefix);

  *conf = newconf;
}

void
preload_conf_dump_log (void)
{
  const char *curgrp = "";
  time_t curtime;
  char *timestr;

  g_message ("conf log dump requested");
  curtime = time (NULL);
  timestr = ctime (&curtime);
  fprintf (stderr, "#\n");
  fprintf (stderr, "# loaded configuration at %s", timestr);
#define print_integer(v, unit) \
	fprintf (stderr, "%d", v / unit);
#define print_enum(v, unit) \
	fprintf (stderr, "%d", v);
#define print_boolean(v, unit) \
	fprintf (stderr, "%s", v ? "true" : "false");
#define print_string(v, unit) \
	fprintf (stderr, "%s", v);
#define print_string_list(v, unit) G_STMT_START {\
	  char **p = v; \
	  if (p) \
	    fprintf (stderr, "%s", *p++); \
	  while (p && *p) \
	    fprintf (stderr, ";%s", *p++); \
	} G_STMT_END 
#define confkey(grp, type, key, def, unit) \
	if (strcmp (STRINGIZE(grp), curgrp)) \
	  fprintf (stderr, "[%s]\n", curgrp = STRINGIZE(grp)); \
	fprintf (stderr, "%s = ", STRINGIZE(key)); \
	print_##type (conf->grp.key, unit); \
	fprintf (stderr, "\n");
#include "confkeys.h"
#undef confkey
  fprintf (stderr, "# loaded configuration - end\n");
  fprintf (stderr, "#\n");
  g_debug ("conf log dump done");
}
