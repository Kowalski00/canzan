#include "glib-object.h"
#include <gtk/gtk.h>
#include "src/resources.c"

#define APP_PREFIX "/com/github/rkj/canzan/"

static void click_start_button_cb(GtkButton *button)
{
}

static void click_repeat_button_cb(GtkButton *button)
{
}

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkBuilder *build;
	GtkWidget *window;

	build = gtk_builder_new_from_resource(APP_PREFIX "menu.ui");
	window = GTK_WIDGET(gtk_builder_get_object(build, "window"));
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	g_object_unref(build);

	gtk_window_present(GTK_WINDOW (window));
}

int main(int argc, char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("org.rkj.anzan", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return status;
}
