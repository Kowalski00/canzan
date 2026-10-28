#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "glibconfig.h"
#include <gtk/gtk.h>
#include <stdlib.h>

#define APP_PREFIX "/com/github/rkj/canzan/"

static double digits = 1;
static double rows = 2;
static int *pCurrentNumbers;
static int *pPastNumbers;
static int counter = 0;

static GtkLabel *mainNumberDisplay;

static int getPow(double digits)
{
	int result = 1;
	for(int i = 0; i < digits; i++) {
		result *= 10;	
	}

	return result;
}

static int getRandomNumber()
{
	if(digits == 0) return 0;
	int max = getPow(digits);
	int min = getPow(digits - 1);
	int randomNumber = min + (rand() % (max - min));
	return randomNumber;
}

static void copyArray(int *source, int *target, int size)
{
	for(int i = 0; i < size; i++) {
		target[i] = source[i];
	}	
}

static gboolean handleSettingNumberLabel(gpointer user_data)
{
	if(counter == rows) {
		pPastNumbers = (int *) malloc(rows * sizeof(int));
		copyArray(pCurrentNumbers, pPastNumbers, rows);
		free(pCurrentNumbers);
		pCurrentNumbers = NULL;
		counter = 0;
		gtk_widget_set_sensitive(GTK_WIDGET(user_data), TRUE);
		gtk_label_set_label(GTK_LABEL(mainNumberDisplay), "");
		return FALSE;
	}
	int randomNumber = pCurrentNumbers[counter];
	int length = snprintf(NULL, 0, "%d", randomNumber);
	char* stringNumber = malloc(length + 1);
	snprintf(stringNumber, length + 1, "%d", randomNumber);

	gtk_label_set_label(GTK_LABEL(mainNumberDisplay), stringNumber);

	free(stringNumber);
	counter++;
	return true;
}

static void handleRepeatButton(GtkButton *button, gpointer user_data)
{

	g_print("pPastNumbers: %p\n", pPastNumbers);
	pCurrentNumbers = pPastNumbers;
	g_print("pCurrentNumbers: %p\n", pCurrentNumbers);
	counter = 0;
	guint id = g_timeout_add(1000, handleSettingNumberLabel, user_data);
}

static void handlePlayButton(GtkButton *button, gpointer user_data)
{
	int randomNumber;
	pCurrentNumbers = (int *) malloc(rows * sizeof(int));
	for(int i = 0; i < rows; i++) {
		randomNumber = getRandomNumber();
		pCurrentNumbers[i] = randomNumber;
	}
	for(int i = 0; i < rows; i++) {
		g_print(" %d ", pCurrentNumbers[i]);
	}
	g_print(" \n ");
	g_print("pCurrentNumbers: %p\n", pCurrentNumbers);
	
	counter = 0;
	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	guint id = g_timeout_add(1000, handleSettingNumberLabel, user_data);
}

static void handleDigitsSpinButton(GtkSpinButton *button, gpointer user_data)
{
	digits = gtk_spin_button_get_value_as_int(button);
}

static void handleRowsSpinButton(GtkSpinButton *button, gpointer user_data)
{
	rows = gtk_spin_button_get_value_as_int(button);
}

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
	GtkSpinButton *digitsSpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "digitsSpinButton"));
	GtkSpinButton *rowsSpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "rowsSpinButton"));
	GtkWidget *settingsWindow = GTK_WIDGET(gtk_builder_get_object(build, "settingsWindow"));

	GtkApplication *app = gtk_window_get_application(GTK_WINDOW(user_data));

	gtk_window_set_application(GTK_WINDOW(settingsWindow), GTK_APPLICATION(app));

	gtk_window_present(GTK_WINDOW(settingsWindow));

	g_signal_connect(digitsSpinButton, "value-changed", G_CALLBACK(handleDigitsSpinButton), NULL);
	g_signal_connect(rowsSpinButton, "value-changed", G_CALLBACK(handleRowsSpinButton), NULL);

	gtk_spin_button_set_value(digitsSpinButton, digits);
	gtk_spin_button_set_value(rowsSpinButton, rows);
}

static void quitActivated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	free(pPastNumbers);
	free(pCurrentNumbers);
	GApplication *app = G_APPLICATION(user_data);
	g_application_quit(app);
}

static gboolean closeWindow(GtkWindow *window, gpointer user_data)
{
	g_application_quit(G_APPLICATION(user_data));
	return TRUE;
}

static void connectMainWindowSignals(GtkBuilder *build)
{
	GtkButton *playButton;
	GtkButton *repeatButton;

	playButton = GTK_BUTTON(gtk_builder_get_object(build, "playButton"));
	repeatButton = GTK_BUTTON(gtk_builder_get_object(build, "repeatButton"));
	mainNumberDisplay = GTK_LABEL(gtk_builder_get_object(build, "mainDisplayNumbers"));

	if(playButton == NULL) {
		g_printerr("[*] ERROR: Could not find play button object inside canzan.ui");
	}
	
	if(repeatButton == NULL) {
		g_printerr("[*] ERROR: Could not find repeat button object inside canzan.ui");
	}

	if(mainNumberDisplay == NULL) {
		g_printerr("[*] ERROR: Could not find repeat button object inside canzan.ui");
	}

	g_signal_connect(playButton, "clicked", G_CALLBACK(handlePlayButton), playButton);
	g_signal_connect(repeatButton, "clicked", G_CALLBACK(handleRepeatButton), playButton);

	pCurrentNumbers = (int *) malloc(rows * sizeof(int));
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
		g_printerr("[*] ERROR: Could not find menu object inside menu.ui");
	}

	connectMainWindowSignals(build);
	
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
