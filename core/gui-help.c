#include <webkit/webkit.h>
#include <string.h>

#include "gui-help.h"
#include "shared.h"


static gboolean close_webview_cb(WebKitWebView* web_view, GtkWidget* window)
{
	(void) web_view;

	gtk_widget_destroy(window);
	return TRUE;
}

#define FILE_URI_SCHEME "file://"

void gui_help_overview_window(GtkWidget *widget, struct ps *ps)
{
	gchar *html_help_path;
	GtkWidget *help_window;
	GtkWidget *scroll_window;
	WebKitWebView *web_view;
	gchar *url;

	(void)widget;

	html_help_path = g_build_filename(DATA_DIR, "help-pages/index.html", NULL);
	url = g_malloc(strlen(html_help_path) + strlen(FILE_URI_SCHEME) + 2);
	sprintf(url, "%s%s", FILE_URI_SCHEME, html_help_path);
	pr_info(ps, "Load help page from: %s\n", url);

	help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(help_window, 600, 400);

	gtk_window_set_default_size(GTK_WINDOW(help_window), 800, 600);

	web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	scroll_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll_window), GTK_WIDGET(web_view));

	g_signal_connect(web_view, "close-web-view", G_CALLBACK(close_webview_cb), help_window);

	gtk_container_add(GTK_CONTAINER(help_window), scroll_window);
	webkit_web_view_load_uri(web_view, url);
	gtk_widget_grab_focus(GTK_WIDGET(web_view));

	gtk_window_set_decorated(GTK_WINDOW(help_window), FALSE);
	gtk_window_set_position((GtkWindow *)help_window, GTK_WIN_POS_CENTER);
	gtk_window_present((GtkWindow *)help_window);
	gtk_widget_show_all((GtkWidget *)help_window);
	gtk_widget_grab_focus(GTK_WIDGET(help_window));

	g_free(url);
	g_free(html_help_path);
}
