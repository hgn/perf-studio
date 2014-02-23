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


static int execute_raw(const char *program, const char **options)
{
	pid_t pid;
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

	pipe(pipefd);

	pid = fork();
	switch (pid) {
	case 0:
		/* close reading end in the child */
		close(pipefd[0]);

		/* send stdout to the pipe */
		dup2(pipefd[1], 1);

		/* send stderr to the pipe */
		dup2(pipefd[1], 2);

		/* this descriptor is no longer needed */
		close(pipefd[1]);

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
		if (waitpid(pid, &child_status, 0 ) == -1 ) {
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


static void executer_thread(gpointer thread_data, gpointer user_data)
{
	int i;
	struct ps *ps = thread_data;

	log_print(LOG_INFO, "executer thread started");

	for (i = 0; i < 10; i++) {
		g_usleep(1000000);
		log_print(LOG_INFO, "calculating");
		g_async_queue_push(executer_queue, ps);
	}
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
	log_print(LOG_INFO, "shutdown executer threads");

	g_async_queue_unref(executer_queue);

	assert(ps);
}


guint timeout_id;

gboolean timeout_function(gpointer user_data)
{
	gpointer thread_data;

	log_print(LOG_INFO, "wait for data");
	thread_data = g_async_queue_try_pop(executer_queue);
	if (!thread_data) {
		log_print(LOG_INFO, "no data received");
	}
	log_print(LOG_INFO, "data received");

	return TRUE;
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

	log_print(LOG_INFO, "Now do analyzed for project!");

	/* push data to run */
	g_thread_pool_push(executer_pool, GUINT_TO_POINTER (1000), NULL);

	/* now executer a timer to check if the thread is finished */
	timeout_id = g_timeout_add(1000, timeout_function, ps);

}


#if 0
/* iterate over all registered modules,
 * check if the particular events are registerd
 * and if so then the module is informed via update()
 */
static events_multiplexer(struct *ps)
{
}

/* called when executer ends and data was produced
 * then this function is called which in turn calls events multiplexer
 */
static events_update()
#endif
