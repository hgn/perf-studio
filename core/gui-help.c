#include <webkit/webkit.h>

#include "gui-help.h"

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
	    gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
	gtk_widget_destroy(window);
	return TRUE;
}


void gui_help_overview_window(GtkWidget *widget, struct ps *ps)
{
	GtkWidget *help_window;


	help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(help_window, 600, 400);

	gtk_window_set_default_size(GTK_WINDOW(help_window), 800, 600);

	WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

	//g_signal_connect(help_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
	g_signal_connect(webView, "close-web-view", G_CALLBACK(closeWebViewCb), help_window);

	gtk_container_add(GTK_CONTAINER(help_window), scrolledWindow);
	webkit_web_view_load_uri(webView, "http://www.perf-studio.com/");
	gtk_widget_grab_focus(GTK_WIDGET(webView));

	gtk_window_set_decorated(GTK_WINDOW(help_window), FALSE);
	gtk_window_set_position((GtkWindow *)help_window, GTK_WIN_POS_CENTER);
	gtk_window_present((GtkWindow *)help_window);
	gtk_widget_show_all((GtkWidget *)help_window);
	gtk_widget_grab_focus(GTK_WIDGET(help_window));
}
