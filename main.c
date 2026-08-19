#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;

	window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "C Anzan");
	gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);

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
