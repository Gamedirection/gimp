/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include "core/core-types.h"

#include "unique.h"

#if !defined(G_OS_WIN32) && !defined(PLATFORM_OSX)
static gboolean  gimp_unique_dbus_open  (const gchar **filenames,
                                         gboolean      as_new);
static gboolean  gimp_unique_dbus_batch_run (const gchar  *batch_interpreter,
                                             const gchar **batch_commands);
#else
static gboolean  gimp_unique_win32_open (const gchar **filenames,
                                         gboolean      as_new);
#endif


gboolean
gimp_unique_open (const gchar **filenames,
                  gboolean      as_new)
{
#ifdef G_OS_WIN32
  return gimp_unique_win32_open (filenames, as_new);
#elif defined (PLATFORM_OSX)
  /* Opening files through "Open with" from other software is likely handled
   * instead by gui_unique_quartz_init() by gtkosx signal handling.
   *
   * Opening files through command lines will always create new process, because
   * dbus is usually not installed by default on macOS (and when it is, it may
   * not work properly). See !808 and #8997.
   */
  return FALSE;
#else
  return gimp_unique_dbus_open (filenames, as_new);
#endif
}

gboolean
gimp_unique_batch_run (const gchar  *batch_interpreter,
                       const gchar **batch_commands)
{
#ifdef G_OS_WIN32
  g_printerr ("Batch commands cannot be run in existing instance in Win32.\n");
  return FALSE;
#elif defined (PLATFORM_OSX)
  /* Running batch commands through command lines will always run in the new
   * process, because dbus is usually not installed by default on macOS (and
   * when it is, it may not work properly). See !808 and #8997.
   */
  return FALSE;
#else
  return gimp_unique_dbus_batch_run (batch_interpreter,
                                     batch_commands);
#endif
}

#ifdef G_OS_WIN32

static gboolean
gimp_unique_win32_open (const gchar **filenames,
                        gboolean      as_new)
{
#ifndef GIMP_CONSOLE_COMPILATION

/*  for the proxy window names  */
#include "gui/gui-unique.h"

  HWND  window_handle = FindWindowW (GIMP_UNIQUE_WIN32_WINDOW_CLASS,
                                     GIMP_UNIQUE_WIN32_WINDOW_NAME);

  if (window_handle)
    {
      COPYDATASTRUCT  copydata = { 0, };

      if (filenames)
        {
          gchar  *cwd   = g_get_current_dir ();
          gint    i;

          for (i = 0; filenames[i]; i++)
            {
              GFile *file;
              file = g_file_new_for_commandline_arg_and_cwd (filenames[i], cwd);

              if (file)
                {
                  gchar *uri = g_file_get_uri (file);

                  copydata.lpData = uri;
                  copydata.cbData = strlen (uri) + 1;  /* size in bytes   */
                  copydata.dwData = (long) as_new;

                  SendMessage (window_handle,
                               WM_COPYDATA, (WPARAM) window_handle, (LPARAM) &copydata);

                  g_free (uri);
                  g_object_unref (file);
                }
              else
                {
                  g_printerr ("conversion to uri failed for '%s'\n",
                              filenames[i]);
                }
            }

          g_free (cwd);
        }
      else
        {
          SendMessage (window_handle,
                       WM_COPYDATA, (WPARAM) window_handle, (LPARAM) &copydata);
        }

      return TRUE;
    }

#endif

  return FALSE;
}

#else

static gboolean
gimp_unique_dbus_open (const gchar **filenames,
                       gboolean      as_new)
{
  (void) filenames;
  (void) as_new;
  return FALSE;
}


static gboolean
gimp_unique_dbus_batch_run (const gchar  *batch_interpreter,
                            const gchar **batch_commands)
{
  (void) batch_interpreter;
  (void) batch_commands;
  return FALSE;
}
#endif  /* G_OS_WIN32 */
