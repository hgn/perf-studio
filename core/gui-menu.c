#include "perf-studio.h"
#include "gui-menu.h"
#include "gui-project-load.h"
#include "gui-amc.h"
#include "gui-help.h"
#include "gui-about.h"


void gui_menu_init(struct ps *ps)
{
	GtkWidget *menubar;
	GtkWidget *filemenu;
	GtkWidget *projectmenu;
	GtkWidget *systemmenu;
	GtkWidget *viewmenu;
	GtkWidget *helpmenu;

	GtkWidget *file;
	GtkWidget *file_quit;

	GtkWidget *view;
	GtkWidget *view_report;
	GtkWidget *project_load;
	GtkWidget *project_unload;
	GtkWidget *project_new;
	GtkWidget *project_manage;

	GtkWidget *project;

	GtkWidget *systemm;
	GtkWidget *system_report;

	GtkWidget *help;
	GtkWidget *help_overview;
	GtkWidget *help_about;

	menubar     = gtk_menu_bar_new();
	gtk_widget_set_name(menubar, "mainmenu");

	filemenu    = gtk_menu_new();
	projectmenu = gtk_menu_new();
	systemmenu  = gtk_menu_new();
	viewmenu    = gtk_menu_new();
	helpmenu    = gtk_menu_new();

	/* File submenues */
	file      = gtk_menu_item_new_with_mnemonic("_File");
	file_quit = gtk_menu_item_new_with_mnemonic("_Quit");
	g_signal_connect(G_OBJECT(file_quit), "activate", G_CALLBACK(gtk_main_quit), NULL);


	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), file_quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

	/* Projects submenues */
	project         = gtk_menu_item_new_with_mnemonic("_Projects");
	project_load    = gtk_menu_item_new_with_mnemonic("_Load project");
	g_signal_connect(G_OBJECT(project_load), "activate", G_CALLBACK(gui_amc_load_project), ps);
	project_unload  = gtk_menu_item_new_with_mnemonic("_Unload current project");
	g_signal_connect(G_OBJECT(project_unload), "activate", G_CALLBACK(gui_amc_unload_project), ps);

	project_new     = gtk_menu_item_new_with_mnemonic("_Create projects");
	project_manage  = gtk_menu_item_new_with_mnemonic("_Manage projects");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(project), projectmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(projectmenu), project_load);
	gtk_menu_shell_append(GTK_MENU_SHELL(projectmenu), project_unload);
	gtk_menu_shell_append(GTK_MENU_SHELL(projectmenu), project_new);
	gtk_menu_shell_append(GTK_MENU_SHELL(projectmenu), project_manage);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), project);

	/* FIXME:
	 * project statistic missing! Displaying how many gigabytes
	 * are used by each project and show what can be deleted (clean
	 * functionality
	 */

	/* System submenues */
	systemm         = gtk_menu_item_new_with_mnemonic("_Systems");
	system_report = gtk_menu_item_new_with_mnemonic("_New");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(systemm), systemmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(systemmenu), system_report);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), systemm);

	/* View submenues */
	view         = gtk_menu_item_new_with_mnemonic("_View");
	view_report =  gtk_menu_item_new_with_mnemonic("_New");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), viewmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), view_report);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);

	/* Help submenues */
	help        = gtk_menu_item_new_with_mnemonic("_Help");
	help_overview  = gtk_menu_item_new_with_mnemonic("_Overview");
	g_signal_connect(G_OBJECT(help_overview), "activate", G_CALLBACK(gui_help_overview_window), ps);
	help_about   = gtk_menu_item_new_with_mnemonic("_About");
	g_signal_connect(G_OBJECT(help_about), "activate", G_CALLBACK(gui_show_about), ps);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), help_overview);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), help_about);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);

	gtk_box_pack_start(GTK_BOX(ps->s.vbox), menubar, FALSE, FALSE, 0);
	gtk_widget_show_all(menubar);

	return;
}
