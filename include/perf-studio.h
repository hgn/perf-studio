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
	gchar **module_paths;
};

struct screen {
	GtkWidget *main_window;
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

	GSList *module_list;
};

#define MSG_LEVEL(ps) ((ps->args.msg_level))
#define MSG_LEVEL_INFO(ps) ((ps->args.msg_level) >= MSG_LEVEL_INFO)
#define MSG_LEVEL_DEBUG(ps) ((ps->args.msg_level) >= MSG_LEVEL_DEBUG)

#define MODULE_NAME_MAX 32
#define MODULE_DESC_MAX 128

struct module {

	/* elements controlled by modules */
	char name[MODULE_DESC_MAX];
	char description[MODULE_DESC_MAX];
	unsigned long version;

	/* list or registered events */
	GSList *event_list;

	void (*unregister_module)(struct ps *, struct module *);
	void *data;

	/* elements opaque to module. Controlled
	 * by caller
	 */
	void *dl_handle;
	char *path_name;
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
