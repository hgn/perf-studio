#ifndef EXECUTER_H
#define EXECUTER_H

#include <unistd.h>

#include "perf-studio.h"

/* functions called from outa space */
void execute_module_triggered_analyze(struct module *module);
int executer_init(struct ps *ps);
void executer_fini(struct ps *ps);


enum {
	EXECUTER_GUI_REPLY_USER_CANCEL
};

struct executer_gui_reply {
	unsigned int type;
};


/**
 * executer_gui_ctx - context between executer and executer gui
 *
 * During execution the execution gui must be initilized, updated
 * and finally destroyed. To provide a common data structure between
 * the raw executer engine and the GUI implementation this context is
 * used.
 */
struct executer_gui_ctx {

	/* functional members */
	struct ps *ps;

	/* the recuring timeout */
	guint timeout_id;

	/* if a process is running the pid != 0 */
	pid_t child_pid;

	/* callback used by executer GUI if the executer
	 * must be informed (e.g. user clicked "cancel")
	 */
	int (*reply_cb)(struct executer_gui_ctx *, struct executer_gui_reply *);

	/* GUI members - private to GUI handler (see gui-executer.c) */
	void *priv_data;
};

/**
 * executer_gui_init - start modal execution dialog
 *
 * If this function is called the module must allocate a modale
 * dialog and must show all execution relevant messages. Updates
 * are received via executer_gui_update().
 *
 * This function return 0 for success or a negative return value
 * in the case of an error.
 */
int executer_gui_init(struct executer_gui_ctx *);


/**
 * executer_gui_free - free all GUI related resources
 *
 * If execution finished (user enforced or other issues) the main
 * resources previously allocated by executer_gui_init() and GUI
 * related must be freed. This function is called and the GUI is
 * required to free all resources.
 */
void executer_gui_free(struct executer_gui_ctx *);



enum {
	EXECUTER_GUI_UPDATE_PROGRESS_CHANGE,
	EXECUTER_GUI_UPDATE_UNKNOWN_PROGRESS
};

struct executer_gui_update_progress_change {
	/* value between 0 % and 100% */
	unsigned int current_percent;
};

struct executer_gui_update_data {
	unsigned int type;
	union {
	struct executer_gui_update_progress_change executer_gui_update_progress_change;
	};
};


/**
 * executer_gui_update - used to update GUI
 *
 * From time to time the GUI must be updated. Depending
 * on information from the executer these updates are send
 * to the executer GUI
 */
int executer_gui_update(struct executer_gui_ctx *, struct executer_gui_update_data *);






#endif /* EXECUTER_H */
