#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wordexp.h>

#include "perf-studio.h"
#include "executer.h"
#include "module-utils.h"
#include "shared.h"
#include "gui-event-generator.h"
#include "log.h"

static GAsyncQueue *executer_queue;
static GThreadPool *executer_pool;

//struct executer_gui_ctx *executer_gui_ctx;


static int execute_raw(struct executer_gui_ctx *executer_gui_ctx,
		       const char *program, const char **options)
{
	int child_status, pipefd[2];
	char buffer[1024];
	wordexp_t result;
        int i, ret;

	ret = 0;

	/* Expand the string for the program to run.  */
	switch (wordexp(program, &result, 0)) {
	case 0:	/* Successful.  */
		break;
	case WRDE_NOSPACE:
		/* If the error was WRDE_NOSPACE,
		   then perhaps part of the result was allocated.  */
		wordfree(&result);
	default: /* Some other error.  */
		return -EINVAL;
	}

	/* Expand the strings specified for the arguments.  */
	for (i = 0; options[i] != NULL; i++) {
		if (wordexp (options[i], &result, WRDE_APPEND)) {
			wordfree (&result);
			return -EINVAL;
		}
	}

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

		execv(result.we_wordv[0], result.we_wordv);
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

		/* we read all input */
		while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {
			log_print(LOG_DEBUG, "child: %s", buffer);
		}

		/* ... we wait here */
		if (waitpid(executer_gui_ctx->child_pid, &child_status, 0 ) == -1 ) {
			log_print(LOG_ERROR, "waitpid error");
		} else {
			if (WIFEXITED(child_status)) {
				int child_ret = WEXITSTATUS(child_status);
				log_print(LOG_DEBUG, "programm exited with status: %d", child_ret);
				assert(child_ret != 100);
			}
		}
		break;
	}

out:
	wordfree(&result);

	return ret;
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
	log_print(LOG_INFO, "executer thread started");

	g_usleep(10000000);
	log_print(LOG_INFO, "program finished");
	g_async_queue_push(executer_queue, GINT_TO_POINTER(PROGRAM_FINISHED));
}


int executer_init(struct ps *ps)
{
	assert(ps);

	log_print(LOG_INFO, "initialize executer threads (%d threads)", 1);

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


static void executer_gui_finish(struct executer_gui_ctx *executer_gui_ctx)
{
	struct ps *ps;

	/* disable running timeouts */
	if (executer_gui_ctx->timeout_id) {
		g_source_remove(executer_gui_ctx->timeout_id);
		executer_gui_ctx->timeout_id = 0;
	}

	executer_gui_free(executer_gui_ctx);
	ps = executer_gui_ctx->ps;
	g_free(executer_gui_ctx);
	executer_gui_ctx = NULL;
	ps->executer_gui_ctx = NULL;
}


static void executer_gui_next(struct executer_gui_ctx *executer_gui_ctx)
{
	struct ps *ps;
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


gboolean timeout_function(gpointer user_data)
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
	log_print(LOG_INFO, "wait for data");
	thread_data = g_async_queue_try_pop(executer_queue);
	if (!thread_data) {
		log_print(LOG_INFO, "no data received");
		assert(executer_gui_ctx);

		if (executer_gui_ctx->state == EXECUTER_STATE_PROCESSING) {
			executer_gui_update_data.type = EXECUTER_GUI_UPDATE_UNKNOWN_PROGRESS;
			executer_gui_update(executer_gui_ctx, &executer_gui_update_data);
		}
		return TRUE;
	}

	log_print(LOG_INFO, "data received");

	type = GPOINTER_TO_UINT(thread_data);
	switch (type) {
	case PROGRAM_FINISHED:
		log_print(LOG_DEBUG, "received program finished");
		/* FIXME: the programm must be stoped, killed */
		executer_gui_finish(executer_gui_ctx);
		return FALSE;
		break;
	default:
		log_print(LOG_INFO, "received unknown signal");
		break;
	}

	return TRUE;
}


int gui_reply_cb(struct executer_gui_ctx *executer_gui_ctx,
		 struct executer_gui_reply *executer_gui_reply)
{
	assert(executer_gui_ctx);
	assert(executer_gui_reply);

	switch (executer_gui_reply->type) {
	case EXECUTER_GUI_REPLY_USER_CANCEL:
		log_print(LOG_DEBUG, "user cancled GUI executer");
		executer_gui_finish(executer_gui_ctx);
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

	log_print(LOG_INFO, "Now do analyzed for project!");

	/* we now start the GUI */
	ps->executer_gui_ctx = g_malloc0(sizeof(struct executer_gui_ctx));
	ps->executer_gui_ctx->ps = module->ps;
	ps->executer_gui_ctx->reply_cb = gui_reply_cb;
	ps->executer_gui_ctx->state = EXECUTER_STATE_WELCOME_SCREEN;
	executer_gui_init(ps->executer_gui_ctx);

	/* push data to run */
	g_thread_pool_push(executer_pool, GUINT_TO_POINTER (1000), NULL);

	/* now executer a timer to check if the thread is finished */
	ps->executer_gui_ctx->timeout_id = g_timeout_add(250, timeout_function, ps->executer_gui_ctx);
	/* we check that a timeout id is never 0 - if not we must make sure
	 * that we add another check to test if the timeout is running */
	assert(ps->executer_gui_ctx->timeout_id);
}

