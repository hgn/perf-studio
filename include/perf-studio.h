/*;
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

#define streq(a, b) (!strcmp((a),(b)))
#define strcaseeq(a, b) (!strcasecmp((a),(b)))
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
        typeof(x) _i = (x);     \
        typeof(y) _j = (y);     \
        (void) (&_i == &_j);    \
        _i < _j ? _i : _j; })

#define max(x,y) ({             \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);    \
        _x > _y ? _x : _y; })

#define min_t(type, x, y) ({                    \
	type __min1 = (x);                      \
	type __min2 = (y);                      \
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({                    \
	type __max1 = (x);                      \
	type __max2 = (y);                      \
	__max1 > __max2 ? __max1: __max2; })

#define clamp(val, min, max) ({                 \
	typeof(val) __val = (val);              \
	typeof(min) __min = (min);              \
	typeof(max) __max = (max);              \
	(void) (&__val == &__min);              \
	(void) (&__val == &__max);              \
	__val = __val < __min ? __min: __val;   \
	__val > __max ? __max: __val; })

#define clamp_t(type, val, min, max) ({         \
	type __val = (val);                     \
	type __min = (min);                     \
	type __max = (max);                     \
	__val = __val < __min ? __min: __val;   \
	__val > __max ? __max: __val; })

#define min3(x, y, z) ({                        \
	typeof(x) _min1 = (x);                  \
	typeof(y) _min2 = (y);                  \
	typeof(z) _min3 = (z);                  \
	(void) (&_min1 == &_min2);              \
	(void) (&_min1 == &_min3);              \
	_min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) : \
	(_min2 < _min3 ? _min2 : _min3); })

#define max3(x, y, z) ({                        \
	typeof(x) _max1 = (x);                  \
	typeof(y) _max2 = (y);                  \
	typeof(z) _max3 = (z);                  \
	(void) (&_max1 == &_max2);              \
	(void) (&_max1 == &_max3);              \
	_max1 > _max2 ? (_max1 > _max3 ? _max1 : _max3) : \
	(_max2 > _max3 ? _max2 : _max3); })


/* determine the size of an array */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BITSIZEOF(x)  (CHAR_BIT * sizeof(x))

#define UNUSED_PARAM __attribute__ ((__unused__))
#define NORETURN __attribute__ ((__noreturn__))
#define PACKED __attribute__ ((__packed__))
#define ALIGNED(m) __attribute__ ((__aligned__(m)))

#if __GNUC_PREREQ(3,0) && !defined(__NO_INLINE__)
# define ALWAYS_INLINE __attribute__ ((always_inline)) inline
# define NOINLINE      __attribute__((__noinline__))
# if !ENABLE_WERROR
#  define DEPRECATED __attribute__ ((__deprecated__))
#  define UNUSED_PARAM_RESULT __attribute__ ((warn_unused_result))
# else
#  define DEPRECATED /* n/a */
#  define UNUSED_PARAM_RESULT /* n/a */
# endif
#else
# define ALWAYS_INLINE inline /* n/a */
# define NOINLINE /* n/a */
# define DEPRECATED /* n/a */
# define UNUSED_PARAM_RESULT /* n/a */
#endif

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#ifdef __COUNTER__
  #define STATIC_ASSERT(e) \
    { enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }; }
#else
  #define STATIC_ASSERT(e) \
    { enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }; }
#endif



enum {
	MSG_LEVEL_DEBUG,
	MSG_LEVEL_INFO,
	MSG_LEVEL_WARNING,
	MSG_LEVEL_ERROR
};
#define DEFAULT_MSG_LEVEL MSG_LEVEL_INFO

enum {
	THEME_DARK,
	THEME_LIGHT
};


struct args {
	gchar *me;
	gchar msg_level;
	guint theme;
	gboolean list_available_modules;
};

/* nested structure reflect ini file style structure */
struct conf {
	struct {
		gchar *perf_path;
		gchar *username;
		gchar **module_paths;
	} common;
	/* section module conf in ~/.config/perf-studio/config */
	struct {
		gboolean show_experimental_modules;
	} module_conf;
	struct {
		gboolean statusbar_enabled;
	} ui;
};

/* forward decl */
struct gt_pie_chart;

struct screen {
	GtkWidget *main_window;

	GtkWidget *main_paned;
	gint main_paned_position;

	GtkWidget *project_load_window;
	// FIXME rename vbox
	GtkWidget *vbox;

	/* modale window to execute a command */
	GtkWidget *event_executer_window;

	struct {
		GtkWidget *label;
	} atitle;
	GtkWidget *statusbar;

	/* APO Section */
	struct {
		GtkWidget *id;
		GtkWidget *description;
		GtkWidget *cmd_path;
		GtkWidget *cmd_args;
		GtkWidget *working_dir;
	} project_info;
	struct {
		GtkWidget *expander;
		GtkWidget *darea;
	} project_info_segment_size;

	/* AMC Section */
	GtkWidget *amc_notebook;
};

struct data {
	struct {
		struct gt_pie_chart *pie_chart_data;
	} project_info_segment_size;
};


enum {
	PROJECT_STATUS_OK = 0,
	PROJECT_STATUS_CMD_PATH_INVALID,
	PROJECT_STATUS_CMD_NOT_EXECUTABLE,
	PROJECT_STATUS_SOMEHOW_INVALID,
};

/* forward declaration, see measurement-class.h */
struct mc_store;

struct project {
	/* values from .perf-studio/config */
	gchar *id;
	gchar *cmd;
	gchar *description;
	gchar *cmd_args_full;
	gchar **cmd_args_splitted;

	int status;

	/* $HOME/.cache/perf-studio/projects/0001 */
	gchar *project_path;

	/* $HOME/.cache/perf-studio/projects/0001/db/8476e28e5.. */
	gchar *project_db_path;

	/* $HOME/.cache/perf-studio/projects/0001/refs */
	gchar *project_refs_path;

	/* current checksum (MD5, SHA1, ...) of executable */
	gchar *checksum;

	/* the registered events for this
	 * prooject. See core/executer.c */
	GSList *events_list;

	/* timestamp where the project was last loaded.
	 * Or 0 if it was never loaded */
	guint64 last_used_timestamp;

	/* if the project has activated measurement registered */
	struct mc_store *mc_store;

	struct ps *ps;
};

/* forward declaration, see cpu-fueatures.{c,h) */
struct cpu_features;

enum {
	FG_COLOR = 0,
	BG_COLOR,
	BG_COLOR_DARKER,

	COLOR_MAX
};

/*
 * Cairo use double, we just use a reduced
 * set of floats to be cacheline friendly
 * The high resolution is not required here
 */
struct ps_color {
	float red;
	float green;
	float blue;
	float alpha;
};

#define Hex8ToFloat(x) ((float)x / 0xff)
#define Hex16ToFloat(x) ((float)x / 0xffff)

static inline void ps_set_source_rgba(cairo_t *cr, struct ps_color *c)
{
	cairo_set_source_rgba(cr, c->red, c->green, c->blue, c->alpha);
}


struct screen_info {
	gint width;
	gint height;
};

struct screen_data {
	gchar *pixmapdir;
	gchar *buttondir;
	gchar *theme_style_path;
	GdkRGBA color[COLOR_MAX];
};

struct ps {
	gboolean screen_usable;
	struct screen s;
	struct screen_info info;
	struct screen_data si;
	struct data d;
	/* CLI args */
	struct args args;
	struct conf conf;
	struct cpu_features *cpu_features;

	/*
	 * current loaded/active project, one
	 * of project_list
	 */
	struct project *active_project;

	/*
	 * Every component can register itself to get informed
    	 * if a projects come active (or deactive). ps->project
    	 * is guaranteed to be a valid pointer. This function
    	 * can be used to change project title in a statusbar, etc
    	 * pp. Module on the other hand MUST not use this function,
    	 * the activation/deactivation for modules is triggered by the
    	 * project - modules are directly informed by the project
    	 */
	GSList *project_activate_cb_list;
	GSList *project_deactivate_cb_list;

	/* list of all available projects */
	GSList *project_list;

	/* list of struct modules found in PATH, ... */
	GSList *module_list;

	GRand *rand;
};

#define MSG_LEVEL(ps) ((ps->args.msg_level))
#define MSG_LEVEL_INFO(ps) ((ps->args.msg_level) >= MSG_LEVEL_INFO)
#define MSG_LEVEL_DEBUG(ps) ((ps->args.msg_level) >= MSG_LEVEL_DEBUG)

#define MODULE_NAME_MAX 32
#define MODULE_DESC_MAX 128

enum {
	MODULE_GROUP_CORE_ANALYSIS = 0,
	MODULE_GROUP_APPLICATION_LEVEL,
	MODULE_GROUP_ARCHITECTURE_LEVEL,

	MODULE_GROUP_MAX
};

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


enum {
	MODULE_MATURITY_STABLE,
	MODULE_MATURITY_EXPERIMENTAL,
};
#define MODULE_MATURITY_DEFAULT MODULE_MATURITY_STABLE



/* forward decl, see events.h */
struct events;

struct module {

	/* elements controlled by modules */
	char name[MODULE_DESC_MAX];
	char description[MODULE_DESC_MAX];
	unsigned long version;

	/* One of MODULE_MATURITY_*, by default
	 * onle stable modules are loaded and can
	 * be used. This can be overwritten by
	 * an entry to module-conf */
	unsigned int maturity;

	/* must be one of MODULE_GROUP_* */
	unsigned int group;

	/* list or registered events */
	struct events *events;

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
	int (*activate)(struct module *m, GtkWidget **);
	int (*deactivate)(struct module *m);
	void (*unregister_module)(struct ps *, struct module *);

	/* command send by the GUI to enable the functionality
	 * for longer/shorter time. Normally the module should
	 * stop active processing and should do not draw anything
	 * furthermore the elements should be sensitive disabled */
	int (*disable)(struct module *m);
	int (*enable)(struct module *m);

	/* true if the module is activated (displayed) or not */
	int activated;
	gint notebook_id;

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



/* Disector Section */
enum disect_error {
	DISECT_ERROR_NO_ERROR
};


struct disect {

	/* former section of disect is filled
	 * by caller, it provides the disector all
	 * required information to just do the parsing
	 */
	guint type;
	gchar *db_path;

	/* If data parsing is done the data is stored here.
	 * Note that data returned from specific disector */
	enum disect_error disect_error;
	union {
		void *data;
	};
};

typedef int (*disect_async_cb)(struct ps *, struct disect *disct);

/* disect-core.c */
struct disect *disect_new(void);
void disect_free(struct ps *, struct disect *disect);
int disect_async(struct ps *, struct disect *, disect_async_cb);


#endif
