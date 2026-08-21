#include "glib-object.h"
#include <gtk/gtk.h>

static void click_start_button_cb(GtkButton *button)
{
}

static void click_repeat_button_cb(GtkButton *button)
{
}

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *mainBox;
	GtkWidget *buttonBox;
	GtkWidget *buttonStart;
	GtkWidget *buttonRepeat;
	GtkWidget *textView;
	GtkTextBuffer *textBuffer;

	gchar *number;

	number = "55";

	window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "C Anzan");
	gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);

	mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_set_homogeneous(GTK_BOX(mainBox), TRUE);

	buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_set_homogeneous(GTK_BOX(buttonBox), TRUE);

	buttonStart = gtk_button_new_with_label("Começar");
	g_signal_connect(buttonStart, "clicked", G_CALLBACK(click_start_button_cb), NULL);

	buttonRepeat = gtk_button_new_with_label("Repetir");
	g_signal_connect(buttonRepeat, "clicked", G_CALLBACK(click_repeat_button_cb), NULL);

	textView = gtk_text_view_new();
	textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));

	gtk_text_buffer_set_text(textBuffer, number, -1);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_WORD_CHAR);

	gtk_window_set_child(GTK_WINDOW(window), mainBox);

	gtk_box_append(GTK_BOX(mainBox), textView);
	gtk_box_append(GTK_BOX(mainBox), buttonBox);
	gtk_box_append(GTK_BOX(buttonBox), buttonStart);
	gtk_box_append(GTK_BOX(buttonBox), buttonRepeat);

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
