#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "glibconfig.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#include "random.h"

#define APP_PREFIX "/com/github/rkj/canzan/"

static double digits = 1;
static double rows = 2;
static int *pCurrentNumbers;
static int *pPastNumbers;
static int counter = 0;
static int shouldCleanBetweenNumbers = 0;
static int isGroupGradeSelected = 0;
static int selectedGroupGradeRow = 0;

static mt_state randomState;

static GtkLabel *mainNumberDisplay;

static int groupASixthGradeGrid[10][15] = {
	{528,160,427,951,719,452,106,843,385,690,724,381,203,579,634}
	, {967,239,650,108,243,758,491,536,702,871,460,629,984,315,870}
	, {482,251,147,598,120,-643,-839,304,987,439,-671,-305,-526,760,215}
    , {106, 543, 928, 710, 954, 329, 267, 695, 514, 870, 308, 796, 632, 807, 481}
    , {815, 302, 491, 852, -169, -401, -958, 576, 740, 183, -235, -673, 340, 927, 264}
    , {360, 829, 213, 308, 497, 932, 589, 690, 147, 674, 850, 201, 765, 148, 756}
    , {769, 420, 195, -513, -854, 508, 274, 421, 963, -307, -631, -175, 286, 840, 392}
    , {241, 952, 309, 756, 487, 360, 617, 508, 873, 420, 196, 689, 204, 795, 138}
    , {654, 516, 740, 495, 536, -785, -320, -197, 201, 873, 124, -482, -968, 319, 203}
    , {185, 730, 698, 246, 809, 953, 721, 370, 617, 164, 902, 596, 480, 342, 875}
};

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
	uint32_t x = random_uint32(&randomState);
	do {
		x = random_uint32(&randomState);
	} while(x >= (RAND_MAX - RAND_MAX % max));
	x %= max;
	return min + (x % (max - min));
}

static void copyArray(int *source, int *target, int size)
{
	for(int i = 0; i < size; i++) {
		target[i] = source[i];
	}	
}

static gboolean handleSettingNumberLabel(gpointer user_data)
{
	if(shouldCleanBetweenNumbers) {
		shouldCleanBetweenNumbers = 0;
		gtk_label_set_label(GTK_LABEL(mainNumberDisplay), "");
		return TRUE;
	}

	int rowsToPlay;
	if(isGroupGradeSelected) {
		rowsToPlay = 15;
	} else {
		rowsToPlay = rows;
	}

	if(counter == rowsToPlay) {
		pPastNumbers = (int *) malloc(rows * sizeof(int));
		copyArray(pCurrentNumbers, pPastNumbers, rows);
		free(pCurrentNumbers);
		pCurrentNumbers = NULL;
		counter = 0;
		shouldCleanBetweenNumbers = 0;
		gtk_widget_set_sensitive(GTK_WIDGET(user_data), TRUE);
		gtk_label_set_label(GTK_LABEL(mainNumberDisplay), "?");
		return FALSE;
	}

	int randomNumber = pCurrentNumbers[counter];
	int length = snprintf(NULL, 0, "%d", randomNumber);
	char* stringNumber = malloc(length + 1);
	snprintf(stringNumber, length + 1, "%d", randomNumber);

	gtk_label_set_label(GTK_LABEL(mainNumberDisplay), stringNumber);

	free(stringNumber);
	counter++;
	shouldCleanBetweenNumbers = 1;
	return TRUE;
}

static void handleRepeatButton(GtkButton *button, gpointer user_data)
{

	g_print("pPastNumbers: %p\n", pPastNumbers);
	pCurrentNumbers = pPastNumbers;
	g_print("pCurrentNumbers: %p\n", pCurrentNumbers);
	counter = 0;
	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	guint id = g_timeout_add(1000, handleSettingNumberLabel, user_data);
}

static void handlePlayButton(GtkButton *button, gpointer user_data)
{
	int rowsToPlay;
	if(isGroupGradeSelected) {
		rowsToPlay = 15;	
	} else {
		rowsToPlay = rows;
	}
	pCurrentNumbers = (int *) malloc(rowsToPlay * sizeof(int));
	for(int i = 0; i < rowsToPlay; i++) {
		if(isGroupGradeSelected) {
			pCurrentNumbers[i] = groupASixthGradeGrid[selectedGroupGradeRow][i];
		} else {
			pCurrentNumbers[i] = getRandomNumber();
		}
	}
	
	gtk_label_set_label(GTK_LABEL(mainNumberDisplay), "");

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

static void handleSelectRowFromGroupGradeButton(GtkButton *pButton, gpointer user_data)
{
	int row = GPOINTER_TO_INT(user_data);
	isGroupGradeSelected = 1;
	selectedGroupGradeRow = row;
}

static void openExercisesActivated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *build = gtk_builder_new_from_resource(APP_PREFIX"data/exercises.ui");
	GtkWidget *exercisesWindow = GTK_WIDGET(gtk_builder_get_object(build, "exercisesWindow"));

	GtkApplication *app = gtk_window_get_application(GTK_WINDOW(user_data));

	GtkGrid *grid = GTK_GRID(gtk_builder_get_object(build, "exerciseGrid"));

	for(int j = 0; j < 10; j++) {

		int headerNumberLength = snprintf(NULL, 0, "%d", j + 1) + 1;
		char* pHeaderNumber = malloc(headerNumberLength + 1);
		snprintf(pHeaderNumber, headerNumberLength, "%d", j + 1);

		char* pHeaderNumberStylized = malloc(headerNumberLength + 3);
		snprintf(pHeaderNumberStylized, headerNumberLength + 3, "%s%s%s", "(", pHeaderNumber, ")");

		GtkLabel *label = GTK_LABEL(gtk_label_new(pHeaderNumberStylized));
		gtk_grid_attach(grid, GTK_WIDGET(label), j, 0, 1, 1);

		free(pHeaderNumber);
		free(pHeaderNumberStylized);

		for(int i = 0; i < 15; i++) {

			char* pStringNumber = "";
			int length = snprintf(NULL, 0, "%d", groupASixthGradeGrid[j][i]);
			pStringNumber = malloc(length + 1);
			snprintf(pStringNumber, length + 1, "%d", groupASixthGradeGrid[j][i]);

			GtkLabel *label = GTK_LABEL(gtk_label_new(pStringNumber));

			gtk_grid_attach(grid, GTK_WIDGET(label), j, i + 1, 1, 1);
			free(pStringNumber);
		}
	}

	for(int i = 0; i < 10; i++) {
		GtkButton *pButton = GTK_BUTTON(gtk_button_new_with_label("Select"));
		gtk_grid_attach(grid, GTK_WIDGET(pButton), i, 17, 1, 1);
		g_signal_connect(pButton, "clicked", G_CALLBACK(handleSelectRowFromGroupGradeButton), GINT_TO_POINTER(i));
	}
	
	gtk_grid_set_column_spacing(grid, 30);

	gtk_window_set_application(GTK_WINDOW(exercisesWindow), GTK_APPLICATION(app));

	gtk_window_present(GTK_WINDOW(exercisesWindow));
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

	gtk_widget_add_css_class(GTK_WIDGET(mainNumberDisplay), "mainLabel");

	g_signal_connect(playButton, "clicked", G_CALLBACK(handlePlayButton), playButton);
	g_signal_connect(repeatButton, "clicked", G_CALLBACK(handleRepeatButton), repeatButton);

	pCurrentNumbers = (int *) malloc(rows * sizeof(int));
}

static void createActions(GtkWidget *mainWindow)
{
	const GActionEntry window_entries[] = {
		{ "openHistory", openHistoryActivated, NULL, NULL, NULL, {0,0,0}}	
		, { "openSettings", openSettingsActivated, NULL, NULL, NULL, {0,0,0}}
		, { "openExercises", openExercisesActivated, NULL, NULL, NULL, {0,0,0}}
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
static void cssParsingError(GtkCssProvider *provider, GtkCssSection *section, GError *error, gpointer user_data)
{
    g_printerr("CSS parsing error: %s\n", error->message);
}

static void provideCss()
{

	GdkDisplay *display = gdk_display_get_default();
	GtkCssProvider *provider = gtk_css_provider_new();

	g_signal_connect(provider, "parsing-error", G_CALLBACK(cssParsingError), NULL);

	gtk_css_provider_load_from_resource(provider, APP_PREFIX"data/styles.css");
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(provider);
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

	provideCss();

	gtk_window_present(GTK_WINDOW(mainWindow));

	g_object_unref(menuBuild);
	g_object_unref(build);

	initialize_state(&randomState, (uint32_t) time(NULL));
}

static void startup(GtkApplication *app, gpointer user_data)
{
	createAppActions(app);
}

int main(int argc, char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("org.rkj.anzan", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return status;
}
