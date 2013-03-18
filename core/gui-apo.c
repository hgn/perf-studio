/* area project overview */

#include <assert.h>

#include "gui-apo.h"
#include "gui-toolkit.h"
#include "shared.h"
#include "str-parser.h"
#include "kv-list.h"


static void widget_set_title(GtkWidget *widget, const char *title)
{
	char buf[128];

	snprintf(buf, sizeof(buf) - 1,
			"<span size=\"x-large\" font_weight=\"thin\" "
			"foreground=\"#777\">%s</span>", title);

	gtk_label_set_markup(GTK_LABEL(widget), buf);
}

static GtkWidget *header_status_widget(struct ps *ps, const char *text)
{
	GtkWidget *hbox;
	GtkWidget *label;

	(void) ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new(NULL);
	widget_set_title(label, text);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	return hbox;
}


static GtkWidget *project_info_widget_new(struct ps *ps)
{
	GtkWidget *event_box;
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *e;
	guint row;

	grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
	row = 0;

	/* spacing cells */
	label = gtk_label_new("  ");
	gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 1, row, 1, 1);

	label = gtk_label_new("     ");
	gtk_grid_attach(GTK_GRID(grid), label, 2, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 3, row, 1, 1);

	label = gtk_label_new("       ");
	gtk_grid_attach(GTK_GRID(grid), label, 4, row, 1, 1);
	row++;


	/* name */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	label = gtk_label_new(" Project Name");
	gtk_widget_set_name(GTK_WIDGET(label), "project_info_label");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(event_box), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.name = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(ps->s.project_info.name), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.name), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.name, TRUE);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), ps->s.project_info.name);
	gtk_grid_attach(GTK_GRID(grid), event_box, 3, row, 1, 1);
	row++;



	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	label = gtk_label_new(" Description ");
	gtk_widget_set_name(GTK_WIDGET(label), "project_info_label");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(event_box), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.description = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(ps->s.project_info.description), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.description), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.description, TRUE);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), ps->s.project_info.description);
	gtk_grid_attach(GTK_GRID(grid), event_box, 3, row, 1, 1);
	row++;


	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	label = gtk_label_new(" Execution Command ");
	gtk_widget_set_name(GTK_WIDGET(label), "project_info_label");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(event_box), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.exec_path = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(ps->s.project_info.exec_path), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.exec_path), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.exec_path, TRUE);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), ps->s.project_info.exec_path);
	gtk_grid_attach(GTK_GRID(grid), event_box, 3, row, 1, 1);
	row++;


	/* exec_args */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	label = gtk_label_new(" Command Arguments ");
	gtk_widget_set_name(GTK_WIDGET(label), "project_info_label");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(event_box), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.exec_args = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(ps->s.project_info.exec_args), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.exec_args), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.exec_args, TRUE);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), ps->s.project_info.exec_args);
	gtk_grid_attach(GTK_GRID(grid), event_box, 3, row, 1, 1);
	row++;


	/* working dir */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	label = gtk_label_new(" Working Directory ");
	gtk_widget_set_name(GTK_WIDGET(label), "project_info_label");
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(event_box), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.working_dir = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(ps->s.project_info.working_dir), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.working_dir), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.working_dir, TRUE);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), ps->s.project_info.working_dir);
	gtk_grid_attach(GTK_GRID(grid), event_box, 3, row, 1, 1);
	row++;

	/* spacing cells */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 2, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 3, row, 1, 1);

	label = gtk_label_new(" ");
	gtk_grid_attach(GTK_GRID(grid), label, 4, row, 1, 1);


	return grid;
}


static gboolean segment_size_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	struct ps *ps;

	ps = data;

	gt_pie_chart_draw(ps, widget, cr, ps->d.project_info_segment_size.pie_chart_data);

	gtk_widget_set_size_request(widget, 100, 75);

	return FALSE;
}


static gboolean segment_size_configure_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	(void)widget;
	(void)event;
	(void)data;

	return FALSE;
}

static GtkWidget *segment_size_darea_create(struct ps *ps)
{
	GtkWidget *darea;
	darea = gtk_drawing_area_new();

	g_signal_connect(darea, "draw", G_CALLBACK(segment_size_draw_cb), ps);
	g_signal_connect(darea, "configure-event", G_CALLBACK(segment_size_configure_cb), ps);

	return darea;
}


static GtkWidget *object_segment_size_widget_new(struct ps *ps)
{
	GtkWidget *expander;
	struct gt_pie_chart *pie_chart;
	const struct ps_color fg_color = {
		.red   = .4,
		.green = .4,
		.blue  = .4,
		.alpha = 1.0
	};

	expander= gtk_expander_new("Section Size");

	ps->s.project_info_segment_size.darea = segment_size_darea_create(ps);

	pie_chart = gt_pie_chart_new();
	gt_pie_chart_set_fg_color(pie_chart, &fg_color);
	gt_pie_chart_set_linewidth(pie_chart, 2);
	ps->d.project_info_segment_size.pie_chart_data = pie_chart;

	gtk_container_add(GTK_CONTAINER(expander), ps->s.project_info_segment_size.darea);
	gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);

	return expander;
}


static GtkWidget *function_size_widget_new(void)
{
	GtkWidget *expander;
	GtkWidget *entry;

	expander= gtk_expander_new("Function Size");

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(expander), entry);
	gtk_expander_set_expanded(GTK_EXPANDER (expander), FALSE);

	return expander;
}


static GtkWidget *apo_main_widget_new(struct ps *ps)
{
	GtkWidget *vbox;
	GtkWidget *header;
	GtkWidget *project_info;
	GtkWidget *widget;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = header_status_widget(ps, " Project Overview");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 2);

	project_info = project_info_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), project_info, FALSE, TRUE, 2);

	header = header_status_widget(ps, " Object Details");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 2);

	widget = object_segment_size_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 4);

	widget = function_size_widget_new();
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 4);


	return vbox;
}


struct kv_list *cmd_segment_size_create(const char *exec_path)
{
        int ret;
	unsigned int i;
        const gchar *argv[] = { "/usr/bin/size", exec_path, NULL };
        gchar *output = NULL;
        GError *error = NULL;
        int exit_status = 0;
        struct kv_list *kv_list;
        const char *keys[] = {"text", "data", "bss" };
        struct str_parser str_parser;
	char label[32];

        if (!g_spawn_sync(NULL, (gchar **)argv, NULL, 0, NULL, NULL,
                          &output, NULL, &exit_status, &error)) {
                // handle error here
                return NULL;
        }
        str_parser_init(&str_parser, output);
	str_parser_skip_line(&str_parser);

        kv_list = kv_list_new(KV_LIST_TYPE_INT_STRING);

        for (i = 0; i < ARRAY_SIZE(keys); i++) {
                long longval;
                ret = str_parser_next_long(&str_parser, &longval);
                if (ret != STR_PARSER_RET_SUCCESS) {
                        goto err;
                }

		snprintf(label, sizeof(label) - 1, "%s: %ld byte", keys[i], longval);
		label[sizeof(label) - 1] = '\0';
		kv_list_add_int_string(kv_list, longval, label);

        }

        g_free(output);
        return kv_list;
err:
        kv_list_free(kv_list);
        g_free(output);

        return NULL;
}


void cmd_segment_size_free(struct kv_list *kv_list)
{
	(void)kv_list;
}


static void gui_apc_update_segment_size(struct ps *ps)
{
        struct kv_list *kv_list;
	struct gt_pie_chart *gt_pie_chart;

        assert(ps);
        assert(ps->project);

        kv_list = cmd_segment_size_create("/usr/bin/gcc");
        if (!kv_list) {
                pr_error(ps, "Cannot get segment size");
                return;
        }
	gt_pie_chart = ps->d.project_info_segment_size.pie_chart_data;
	assert(gt_pie_chart);

	gt_pie_chart_set_data(gt_pie_chart, kv_list);

	/* force redraw */
	gtk_widget_queue_draw_area(ps->s.project_info_segment_size.darea, 0, 0, -1, -1);
}


/*
 * ps->project is new (or replaced), update
 * all views and related fields now
 */
void gui_apo_new_project_loaded(struct ps *ps)
{
	assert(ps->project);
        /* update project summary fields */

        /* update exec segment view generated via size(1) */
        gui_apc_update_segment_size(ps);
}


GtkWidget *gui_apo_new(struct ps *ps)
{
	GtkWidget *scroll_widget;
	GtkWidget *main_widget;


	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);

	main_widget = apo_main_widget_new(ps);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), main_widget);

	return scroll_widget;
}
