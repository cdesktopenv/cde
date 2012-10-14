/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: UserPreference.C /main/7 1998/04/17 11:38:58 mgreess $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#define C_UserPreference
#define L_Preferences

//#ifndef TEST_MODE
#define C_EnvMgr
#define C_MessageMgr
#define L_Managers
//#endif

#include <Prelude.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#if !defined(hpux) && !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
#include <sysent.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>

PreferenceRecord *PreferenceRecord::g_head;
PreferenceRecord *PreferenceRecord::g_tail;
int PreferenceRecord::g_update_count = -1;


PreferenceRecord::PreferenceRecord (const char *key, const char *value)
: f_key (strdup (key)),
  f_value (strdup (value)),
  f_next (NULL)
{
  if (g_head == NULL)
    g_head = this;
  if (g_tail != NULL)
    g_tail->f_next = this;
  g_tail = this;
}


PreferenceRecord::~PreferenceRecord()
{
  free ((char *) f_key);
  free ((char *) f_value);
}


void
PreferenceRecord::set_value (const char *value)
{
  free ((char *) f_value);
  f_value = strdup (value);
}


const char *
PreferenceRecord::form_filename()
{
  // Form the file name of the preferences.
  static char filename[256];
  if (filename[0] == '\0')
    {
      sprintf (filename, "%s/preferences", env().user_path());
#if EAM
      const char *home = env().home();
      if (home == NULL)
	throw (CASTEXCEPT Exception());
      const char *lang = env().lang();
      if (lang == NULL)
	throw (CASTEXCEPT Exception());

      sprintf (filename, "%s/.dt/dtinfo/%s/preferences",
                         home, lang);
#endif
    }

  return (filename);
}


void
revert_from_backup (const char *filename)
{
  int ret;
  // Failed, so look for the backup file.
  char backup[256], original[256];
  sprintf (backup, "%s.bak", filename);
  struct stat file_info;

  if (stat (backup, &file_info) != -1 &&
      S_ISREG(file_info.st_mode))
    {
      unlink (filename);
      ret = link (backup, filename);
    }
}


int
read_version (FILE *stream)
{
  size_t ret1;
  int ret2;
  // Make sure the file is valid.
  char V = '-';
  ret1 = fread (&V, 1, 1, stream);
  if (V != 'V')
    return (0);
  // Nab the version from the file. 
  int version = 0;
  ret2 = fscanf (stream, "%d", &version);
  return (version);
}


int
read_update_count (FILE *stream)
{
  char *ret;
  char buffer[256], *p;
  int update_count;

  ret = fgets (buffer, 256, stream);
  p = buffer;
  while (*p != ',' && *p != '\0')
    p++;
  p++;
  if (*p == '\0')
    return (0);
  sscanf (p, "%d", &update_count);
  ON_DEBUG (printf ("Update count = %d\n", update_count));
  return (update_count);
}

void
PreferenceRecord::read_prefs()
{
  // Open it and read in the preferences.
  const char *filename;
  FILE *stream;
  int attempt = 0;

  while (attempt < 2)
    {
      attempt++;
      filename = form_filename();
      // See if it exists.
      struct stat file_info;
      int status = stat (filename, &file_info);
      if (status == -1)
	{
	  // Check for access failure or IO error. 
	  if (errno == EACCES || errno == EIO)
	    throw (CASTEXCEPT Exception());
	  // It doesn't exists otherwise. 
	  g_update_count = 0;
	  return;
	}
      else if (!S_ISREG (file_info.st_mode))
	throw (CASTEXCEPT Exception());
      stream = fopen (filename, "r");
      if (stream == NULL)    // Open failed, something is bogus. 
	throw (CASTEXCEPT Exception());

      int version = read_version (stream);   // Snag the version.
      if (version == 0)                      // See if file is ok. 
	{
	  fclose (stream);
	  if (attempt == 2)   // give up on 2nd attempt 
	    throw (CASTEXCEPT Exception());
	  else
	    revert_from_backup (filename);
	}
    }

  g_update_count = read_update_count (stream);

  // Read in the preference lines. 
  char key[256], *value;
  while (fgets (key, 256, stream) != NULL)
    {
      value = key;
      while (*value != ':' && *value != '\0')
	value++;
      if (*value == '\0')  // Ignore bogus lines. 
	continue;
      *value++ = '\0';
      while (isspace (*value))
	value++;
      int len = strlen(value);
      value[len-1] = '\0';
      ON_DEBUG (printf ("Got Pref: %s = %s\n", key, value));
      new PreferenceRecord (key, value);
    }
  fclose (stream);
}


void
PreferenceRecord::write_prefs()
{
  assert (g_update_count != -1);

  const char *filename = form_filename();
  struct stat file_info;
  int status = stat (filename, &file_info);
  // Make sure it's a regular file if it exists. 
  if (status == 0  && !S_ISREG (file_info.st_mode))
    throw (CASTEXCEPT Exception());

  // Check the update count to guard against overwrites.
  // This isn't foolproof, but we can't trust NFS locking
  // so this will have to do for now.  13:39 15-Sep-93 DJB
  FILE *stream;
  int update_count = 0;

  if (status == 0)
    {
      // If the file isn't readable, we won't write the prefs. 
      stream = fopen (filename, "r");
      if (stream == NULL)
	throw (CASTEXCEPT Exception());

      if (read_version (stream) != 0)
	update_count = read_update_count (stream);
      fclose (stream);
    }

  if (update_count != g_update_count)
    {
      bool doit = message_mgr().
	question_dialog ((char*)"Preferences have changed on disk.\nOverwrite?");
      if (!doit)
	return;
    }

  // Create a backup file from the current preferences, if any.
  char backup[256];
  backup[0] = '\0';
  if (status == 0)
    {
      sprintf (backup, "%s.bak", filename);
      unlink (backup);
      if (rename (filename, backup) == -1)
	throw (CASTEXCEPT Exception());
    }
  else    // Make sure the parent directory exists.
    {
      char dirname[256];
      sprintf (dirname, "%s", env().user_path());
      status = stat (dirname, &file_info);
      if (status == -1)
	{
	  if (mkdir (dirname, 0777) == -1)
	    throw (CASTEXCEPT Exception());
	}
      else if (!S_ISDIR (file_info.st_mode))
	throw (CASTEXCEPT Exception());
    }

  // Open the file and write the preferences.
  stream = fopen (filename, "w");
  if (stream == NULL)
    {
      // Put the backup file back where it came from.
      if (backup[0] != '\0')
	rename (backup, filename);
      throw (CASTEXCEPT Exception());
    }

  // Write out the preference records. 
  PreferenceRecord *current = g_head;
  // First line is version and update count.
  fprintf (stream, "-1.0, %d  # AUTOMATICALLY GENERATED - DO NOT EDIT!\n",
	   ++g_update_count);
  while (current != NULL)
    {
      if (*(current->f_value) != '\0' &&
	  fprintf(stream, "%s: %s\n", current->f_key, current->f_value) == EOF)
	{
	  fclose (stream);
	  // Remove the file and restore the backup file. 
	  if (unlink (filename) == 0 && backup[0] != '\0')
	    rename (backup, filename);
	  throw (CASTEXCEPT Exception());
	}
      current = current->f_next;
    }

  // Rewrite first character to indicate file is complete. 
  fseek (stream, 0L, 0);
  fwrite ("V", 1, 1, stream);

  fclose (stream);
}


PreferenceRecord *
PreferenceRecord::lookup (const char *key)
{
  if (g_update_count == -1)
    {
      mtry
	{
	  read_prefs();
	}
      mcatch_any()
	{
	  // This will only happen the first time through. 
	  message_mgr().error_dialog ((char*)"Unable to read preferences.");
	  g_update_count = 0;
	}
      end_try;
    }

  // Scan through the list of preferences looking for the record. 
  PreferenceRecord *current = g_head;
  while (current != NULL)
    {
      if (strcmp (key, current->f_key) == 0)
	return (current);
      current = current->f_next;
    }

  // Not found, so create a new record.
  return (new PreferenceRecord (key, ""));
}


UserPreference::UserPreference (const char *key)
  : f_preference (PreferenceRecord::lookup (key))
{
}


UserPreference::~UserPreference()
{
}
