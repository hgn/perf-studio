#include "perf-studio.h"
#include "shared.h"

typedef struct
{
  gchar *name, *hello;
  gboolean *boolean;
  int *nums;
  gchar **strings;
  int meaning_of_life;
  gdouble *doubles;
} Settings;


int load_conf_file(struct ps *ps)
{
	//Settings *conf;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	gsize length;

	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, "perf-studio.conf", flags, &error)) {
		//g_error("%s", error->message);
		pr_info(ps, "failed to open configuration file");
		goto out;
	}


	/* Read in data from the key file from the group "username". */
	ps->conf.module_paths = g_key_file_get_string_list(keyfile, "modules", "path", &length, NULL);

out:
	return 0;
}
