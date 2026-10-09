#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "glibconfig.h"
#include <gtk/gtk.h>

#define APP_PREFIX "/com/github/rkj/canzan/"

static void openHistoryActivated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *build = gtk_builder_new_from_resource(APP_PREFIX"data/history.ui");
	GtkWidget *historyWindow = GTK_WIDGET(gtk_builder_get_object(build, "historyWindow"));

	GtkApplication *app = gtk_window_get_application(GTK_WINDOW(user_data));

	gtk_window_set_application(GTK_WINDOW(historyWindow), GTK_APPLICATION(app));

	gtk_window_present(GTK_WINDOW(historyWindow));
}

static void openSettingsActivated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *build = gtk_builder_new_from_resource(APP_PREFIX"data/settings.ui");
	GtkWidget *settingsWindow = GTK_WIDGET(gtk_builder_get_object(build, "settingsWindow"));

	GtkApplication *app = gtk_window_get_application(GTK_WINDOW(user_data));

	gtk_window_set_application(GTK_WINDOW(settingsWindow), GTK_APPLICATION(app));

	gtk_window_present(GTK_WINDOW(settingsWindow));
}

static void quitActivated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GApplication *app = G_APPLICATION(user_data);
	g_application_quit(app);

}

static gboolean closeWindow(GtkWindow *window, gpointer user_data)
{
	g_application_quit(G_APPLICATION(user_data));
	return TRUE;
}

static void createActions(GtkWidget *mainWindow)
{
	const GActionEntry window_entries[] = {
		{ "openHistory", openHistoryActivated, NULL, NULL, NULL, {0,0,0}}	
		, { "openSettings", openSettingsActivated, NULL, NULL, NULL, {0,0,0}}	
	};

	g_action_map_add_action_entries(G_ACTION_MAP(mainWindow), window_entries, G_N_ELEMENTS(window_entries), mainWindow);
}

static void createAppActions(GtkApplication *app)
{
	const GActionEntry window_entries[] = {
		{ "quit", quitActivated, NULL, NULL, NULL, {0,0,0}}	
	};

	g_action_map_add_action_entries(G_ACTION_MAP(app), window_entries, G_N_ELEMENTS(window_entries), app);
}

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkBuilder *build;
	GtkBuilder *menuBuild;
	GtkWidget *mainWindow;
	GMenuModel *menu;
	GtkWidget *menuBar;

	build = gtk_builder_new_from_resource(APP_PREFIX"data/canzan.ui");
	mainWindow = GTK_WIDGET(gtk_builder_get_object(build, "window"));

	g_signal_connect(mainWindow, "close-request", G_CALLBACK(closeWindow), app);
	gtk_window_set_application(GTK_WINDOW(mainWindow), GTK_APPLICATION(app));

	menuBuild = gtk_builder_new_from_resource(APP_PREFIX"data/menu.ui");

	menu = G_MENU_MODEL(gtk_builder_get_object(menuBuild, "menu"));
	menuBar = GTK_WIDGET(gtk_builder_get_object(build, "menuBar"));

	if(menu != NULL && menuBar != NULL) {
		gtk_popover_menu_bar_set_menu_model(GTK_POPOVER_MENU_BAR(menuBar), menu);
	} else {
		g_printerr("[*] WARNING: Could not find menu object inside menu.ui");
	}
	
	createActions(mainWindow);

	gtk_window_present(GTK_WINDOW(mainWindow));

	g_object_unref(menuBuild);
	g_object_unref(build);
}

static void startup(GtkApplication *app, gpointer user_data)
{
	createAppActions(app);
}

int main(int argc, char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("org.rkj.anzan", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return status;
}
