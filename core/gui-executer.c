#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "executer.h"
#include "shared.h"
#include "log.h"

struct executer_gui_priv_data {
};


int executer_gui_init(struct executer_gui_ctx *executer_gui_ctx)
{
	struct executer_gui_priv_data *priv_data;

	assert(executer_gui_ctx);
	assert(executer_gui_ctx->ps);
	assert(executer_gui_ctx->reply_cb);
	assert(!executer_gui_ctx->priv_data);

	log_print(LOG_DEBUG, "executer GUI now started, try to initialize subsystem");

	/* allocate our private data structure */
	priv_data = g_malloc0(sizeof(*priv_data));

	return 0;
}


void executer_gui_free(struct executer_gui_ctx *executer_gui_ctx)
{
	assert(executer_gui_ctx);
	assert(executer_gui_ctx->ps);
	assert(executer_gui_ctx->priv_data);

	log_print(LOG_DEBUG, "executer GUI now released and resources freed");

	/* allocate our private data structure */
	g_free(executer_gui_ctx->priv_data);
	executer_gui_ctx->priv_data = NULL;
}


int executer_gui_update(struct executer_gui_ctx *executer_gui_ctx,
			struct executer_gui_update *executer_gui_update)
{
	assert(executer_gui_ctx);
	assert(executer_gui_update);

	log_print(LOG_DEBUG, "update GUI");

	return 0;
}
