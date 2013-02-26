/*
** Copyright (C) 2013 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef PERF_STUDIO_H
#define PERF_STUDIO_H

#include <glib.h>
#include <gtk/gtk.h>

enum {
	MSG_LEVEL_DEBUG,
	MSG_LEVEL_INFO,
	MSG_LEVEL_WARNING,
	MSG_LEVEL_ERROR
};

#define DEFAULT_MSG_LEVEL MSG_LEVEL_INFO


struct args {
	gchar *me;
	gchar msg_level;
	gboolean list_available_modules;
};

struct conf {
	gchar *perf_exec_path;
	gchar **module_paths;
};

struct screen {
	GtkWidget *main_window;
};

struct project {
	/* values from .perf-studio/config */
	char *exec_path;
	GSList *exec_args;

	/* current sha1 of executable */
	unsigned long sha1;
};

/* forward declaration, see cpu-fueatures.{c,h) */
struct cpu_features;

struct ps {
	gboolean screen_usable;
	struct screen s;
	/* CLI args */
	struct args args;
	struct conf conf;
	struct cpu_features *cpu_features;

	/*
	 * current loaded/active project, one
	 * of project_list
	 */
	struct project *project;
	/* list of all available projects */
	GSList *project_list;

	/* list of struct modules found in PATH, ... */
	GSList *module_list;
};

#define MSG_LEVEL(ps) ((ps->args.msg_level))
#define MSG_LEVEL_INFO(ps) ((ps->args.msg_level) >= MSG_LEVEL_INFO)
#define MSG_LEVEL_DEBUG(ps) ((ps->args.msg_level) >= MSG_LEVEL_DEBUG)

#define MODULE_NAME_MAX 32
#define MODULE_DESC_MAX 128

enum {
	MODULE_GROUP_COMMON = 0,
	MODULE_GROUP_THREAD_ANALYSE,
};
#define MODULE_GROUP_DEFAULT MODULE_GROUP_COMMON

enum update_type {
	UPDATE_TYPE_PERF_DATA_PATH,
	/*
	 * note that several path to perf.data files can
	 * be provided. Just to do a more accurate event
	 * sampling because of performance register pressure.
	 * This decision is enforced by the core. E.g if two
	 * other modules took seperate measurements. There is
	 * no need to measure the data one more time just to
	 * collect the data in one perf.data
	 */
	UPDATE_TYPE_PERF_DATA_PATHS,
};

struct module {

	/* elements controlled by modules */
	char name[MODULE_DESC_MAX];
	char description[MODULE_DESC_MAX];
	unsigned long version;

	/* must be one of MODULE_GROUP_* */
	unsigned int module_group;

	/* list or registered events */
	GSList *event_list;

	/*
	 * update receive data and should transform
	 * data in a module specific format, just to
	 * display the data as quick as possible
	 */
	int (*update)(struct module *m, enum update_type update_type, ...);

	/*
	 * Activate the gui element and start display.
	 * If no data is available (update() wasn't called)
	 * the a black screen MUST be displayed */
	int (*activate)(struct module *m);
	int (*deactive)(struct module *m);
	void (*unregister_module)(struct ps *, struct module *);

	/*
	 * module private data - must be freed at
	 * unregister_module() time
	 */
	void *data;

	/* elements opaque to module. Controlled
	 * by caller
	 */
	void *dl_handle;
	char *path_name;

	/*
	 * Back reference to ps, module is instance object (thus argument)
	 * and ps pointer points to superclass element.
	 */
	struct ps *ps;
};





/* module specific section */
#define LOCAL_MODULES_DIR ".config/perf-studio/modules"
#define PERF_STUDIO_MODULE_REGISTER_FUNC "register_module"
typedef int (*module_register_fn_t)(struct ps *, struct module **);

/* event specific data */

enum {
	EVENT_TYPE_COUNTER = 0,
	EVENT_TYPE_SAMPLING,

	EVENT_TYPE_MAX
};

enum {
	USERKERNELSPACE,
	USERSPACE,
	KERNELSPACE,
	HYPERVISOR,
};

struct event_counting {
	int event;
	int where;
};

struct event_sampling {
	int event;
	int where;
};

struct event {
	int type;
	union {
		struct event_sampling sampling;
		struct event_counting counting;
	};
};

#define streq(a, b) (!strcmp((a),(b)))
# if !defined likely && !defined unlikely
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
# endif

/* for CHAR_BITS */
#include <limits.h>

#undef __always_inline
#if __GNUC_PREREQ (3,2)
# define __always_inline __inline __attribute__ ((__always_inline__))
#else
# define __always_inline __inline
#endif

/*
 * See if our compiler is known to support flexible array members.
 */
#ifndef FLEX_ARRAY
#if defined(__STDC_VERSION__) && \
	(__STDC_VERSION__ >= 199901L) && \
	(!defined(__SUNPRO_C) || (__SUNPRO_C > 0x580))
# define FLEX_ARRAY /* empty */
#elif defined(__GNUC__)
# if (__GNUC__ >= 3)
#  define FLEX_ARRAY /* empty */
# else
#  define FLEX_ARRAY 0 /* older GNU extension */
# endif
#endif
#ifndef FLEX_ARRAY
# define FLEX_ARRAY 1
#endif
#endif

#define min(x,y) ({             \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);    \
        _x < _y ? _x : _y; })

#define max(x,y) ({             \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);    \
        _x > _y ? _x : _y; })

/* determine the size of an array */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BITSIZEOF(x)  (CHAR_BIT * sizeof(x))

#endif
