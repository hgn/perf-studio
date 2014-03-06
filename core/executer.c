#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "perf-studio.h"
#include "executer.h"
#include "module-utils.h"
#include "shared.h"
#include "gui-event-generator.h"
#include "log.h"
#include "mc.h"

static GAsyncQueue *executer_queue;
static GThreadPool *executer_pool;

//struct executer_gui_ctx *executer_gui_ctx;


static int execute_raw_direct(struct executer_gui_ctx *executer_gui_ctx,
			      char **cmd)
{
	int child_status, pipefd[2];
        int ret;
	char buffer[1024];
	gchar *str;

	ret = 0;

	str = g_strjoinv(" ", cmd);
	log_print(LOG_INFO, "Execute: %s", str);
	g_free(str);

	/* flush current buffer */
	fflush(stdout);
	fflush(stderr);

	pipe(pipefd);

	executer_gui_ctx->child_pid = fork();
	switch (executer_gui_ctx->child_pid) {
	case 0:
		/* close reading end in the child */
		close(pipefd[0]);

		/* send stdout to the pipe */
		dup2(pipefd[1], 1);

		/* send stderr to the pipe */
		dup2(pipefd[1], 2);

		/* this descriptor is no longer needed */
		close(pipefd[1]);

		setvbuf(stdout, NULL, _IOLBF, 0);
		setvbuf(stderr, NULL, _IOLBF, 0);

		execv(cmd[0], (char * const*)cmd);
		log_print(LOG_ERROR, "Failed to execute program");
		exit(100);
	case -1:
		log_print(LOG_ERROR, "Failed to fork");
		ret = -EINVAL;
		goto out;
		break;
	default:
		/* close the write end of the pipe in the parent */
		close(pipefd[1]);

		/* we read all input and print to stderr */
		while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {
			log_print(LOG_INFO, "child: %s", buffer);
		}

		/* ... we wait here */
		if (waitpid(executer_gui_ctx->child_pid, &child_status, 0 ) == -1 ) {
			log_print(LOG_ERROR, "waitpid error");
		} else {
			if (WIFEXITED(child_status)) {
				int child_ret = WEXITSTATUS(child_status);
				log_print(LOG_INFO, "Programm exited with status: %d", child_ret);
				assert(child_ret != 100);
			}
		}
		/* finished program execution, set to 0 */
		executer_gui_ctx->child_pid = 0;
		break;
	}

out:
	return ret;
}


static int execute_raw(struct mc_store *mc_store,
		       struct executer_gui_ctx *executer_gui_ctx)
{
	int ret;
	GSList *tmp;
	struct mc_element *mc_element;

	assert(mc_store);

	tmp = mc_store->mc_element_list;
	while (tmp) {
		mc_element = tmp->data;
		assert(mc_element);
		assert(mc_element->exec_cmd);

		ret = execute_raw_direct(executer_gui_ctx, mc_element->exec_cmd);
		if (ret) {
			log_print(LOG_DEBUG, "failed to execute program, status: %d", ret);
		}

		tmp = g_slist_next(tmp);
	}

	return 0;
}

/*
 * If we wan't to marshal integer we must
 * make sure the value is a valid pointer - not
 * a NULL pointer.
 */
enum {
	PROGRAM_FINISHED = 1
};


/*
 * This function is called if a binary must be analyzed.
 * This funtion blocks until the execution ends.
 */
static void executer_thread(gpointer thread_data, gpointer user_data)
{
	int ret;
	struct ps *ps;
	struct executer_gui_ctx *executer_gui_ctx;

	log_print(LOG_INFO, "executer thread started");

	ps = user_data;
	executer_gui_ctx = thread_data;
	assert(ps && executer_gui_ctx);

	log_print(LOG_INFO, "program start");
	assert(ps);
	assert(ps->active_project);
	assert(ps->active_project->mc_store);
	ret = execute_raw(ps->active_project->mc_store, executer_gui_ctx);
	if (ret) {
		log_print(LOG_INFO, "failed in program execution");
	}
	log_print(LOG_INFO, "program finished");
	g_async_queue_push(executer_queue, GINT_TO_POINTER(PROGRAM_FINISHED));
}


int executer_init(struct ps *ps)
{
	assert(ps);

	log_print(LOG_INFO, "Initialize executer thread pool (%d threads)", 1);

	executer_pool = g_thread_pool_new(executer_thread, ps, 1, FALSE, NULL);

	executer_queue = g_async_queue_new();
	g_async_queue_ref(executer_queue);

	return 0;
}


void executer_fini(struct ps *ps)
{
	assert(ps);
	log_print(LOG_INFO, "shutdown executer threads");
	g_async_queue_unref(executer_queue);
}


static void terminate_running_cmd(struct executer_gui_ctx *executer_gui_ctx)
{
	int ret;

	assert(executer_gui_ctx);

	if (executer_gui_ctx->child_pid == 0) {
		log_print(LOG_INFO, "Program not running anymore");
		return;
	}

	ret = kill(executer_gui_ctx->child_pid, SIGKILL);
	if (ret) {
		log_print(LOG_ERROR, "Could not kill process: %s",
			  strerror(errno));
	}
}



/* This finish GUI and execution context */
static void executer_finish(struct executer_gui_ctx *executer_gui_ctx)
{
	struct ps *ps;

	/* disable running timeouts */
	if (executer_gui_ctx->timeout_id) {
		g_source_remove(executer_gui_ctx->timeout_id);
		executer_gui_ctx->timeout_id = 0;
	}

	/* kill process if still running */
	terminate_running_cmd(executer_gui_ctx);

	executer_gui_free(executer_gui_ctx);
	ps = executer_gui_ctx->ps;
	g_free(executer_gui_ctx);
	executer_gui_ctx = NULL;
	ps->executer_gui_ctx = NULL;
}


static void executer_gui_next(struct executer_gui_ctx *executer_gui_ctx)
{
	struct executer_gui_update_data executer_gui_update_data;

	memset(&executer_gui_update_data, 0, sizeof(executer_gui_update_data));

	switch (executer_gui_ctx->state) {
	case EXECUTER_STATE_WELCOME_SCREEN:
		executer_gui_update_data.type = EXECUTER_GUI_UPDATE_DISPLAY_ANALYSIS;
		executer_gui_update(executer_gui_ctx, &executer_gui_update_data);
		executer_gui_ctx->state = EXECUTER_STATE_PROCESSING;
		break;
	default:
		assert(0);
		break;
	}
}


static gboolean timeout_function(gpointer user_data)
{
	unsigned int type;
	gpointer thread_data;
	struct executer_gui_ctx *executer_gui_ctx;
	struct executer_gui_update_data executer_gui_update_data;

	assert(user_data);
	executer_gui_ctx = user_data;

	memset(&executer_gui_update_data, 0, sizeof(executer_gui_update_data));

	/*
	 * check if data is available from the executed program,
	 * if not we simple send a update message to the gui
	 * to pulse the progress bar
	 */
	thread_data = g_async_queue_try_pop(executer_queue);
	if (!thread_data) {
		assert(executer_gui_ctx);

		if (executer_gui_ctx->state == EXECUTER_STATE_PROCESSING) {
			executer_gui_update_data.type = EXECUTER_GUI_UPDATE_UNKNOWN_PROGRESS;
			executer_gui_update(executer_gui_ctx, &executer_gui_update_data);
		}
		return TRUE;
	}

	type = GPOINTER_TO_UINT(thread_data);
	switch (type) {
	case PROGRAM_FINISHED:
		log_print(LOG_DEBUG, "received program finished");
		/* FIXME: the programm must be stoped, killed */
		executer_finish(executer_gui_ctx);
		return FALSE;
		break;
	default:
		log_print(LOG_INFO, "received unknown signal");
		break;
	}

	return TRUE;
}


static int gui_reply_cb(struct executer_gui_ctx *executer_gui_ctx,
			struct executer_gui_reply *executer_gui_reply)
{
	assert(executer_gui_ctx);
	assert(executer_gui_reply);

	switch (executer_gui_reply->type) {
	case EXECUTER_GUI_REPLY_USER_CANCEL:
		log_print(LOG_DEBUG, "user cancled GUI executer");
		executer_finish(executer_gui_ctx);
		break;
	case EXECUTER_GUI_REPLY_USER_NEXT:
		log_print(LOG_DEBUG, "user clicked next");
		executer_gui_next(executer_gui_ctx);
		break;
	default:
		assert(0);
		break;
	}

	return 0;
}


/*
 * This function is called when a module hit the "start analyze"
 * button. We first check if a project is loaded, if not we simple
 * return. If a project is loaded we now enforce a new execution.
 * In future releases we may check if the data is up to date. If so
 * we can show a "should we really do a new analyze" button. This
 * may be overwritten by configuration.
 */
void execute_module_triggered_analyze(struct module *module)
{
	int ret;
	struct ps *ps;

	assert(module);
	assert(module->ps);
	assert(executer_pool);

	ps = module->ps;

	if (!ps->active_project) {
		log_print(LOG_INFO, "No project loaded - cannot do analyzes for project none");
		return;
	}

	if (ps->executer_gui_ctx) {
		log_print(LOG_ERROR, "Execution already running - cannot start two");
		return;
	}

	/* prepare command to execute */
	assert(ps->active_project != NULL);
	assert(ps->active_project->mc_store != NULL);
	ret = mc_store_update_exec_cmds(ps, ps->active_project->mc_store);
	if (ret) {
		log_print(LOG_ERROR, "Failed to construct excution command");
		return;
	}

	log_print(LOG_INFO, "Now do analyzed for project!");

	/* we now start the GUI */
	ps->executer_gui_ctx = g_malloc0(sizeof(struct executer_gui_ctx));
	ps->executer_gui_ctx->ps = module->ps;
	ps->executer_gui_ctx->reply_cb = gui_reply_cb;
	ps->executer_gui_ctx->state = EXECUTER_STATE_WELCOME_SCREEN;
	executer_gui_init(ps->executer_gui_ctx);

	/* push data to run */
	g_thread_pool_push(executer_pool, ps->executer_gui_ctx, NULL);

	/* now executer a timer to check if the thread is finished */
	ps->executer_gui_ctx->timeout_id = g_timeout_add(250, timeout_function, ps->executer_gui_ctx);
	/* we check that a timeout id is never 0 - if not we must make sure
	 * that we add another check to test if the timeout is running */
	assert(ps->executer_gui_ctx->timeout_id);
}

