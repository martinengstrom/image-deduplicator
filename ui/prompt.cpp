#include "prompt.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <windows.h>
#elif __linux__
    #include <gtk/gtk.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
bool win32Prompt(std::string title, std::string body) {
    int result = MessageBox(NULL, body.c_str(), title.c_str(), 
        MB_OKCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1);

    // Return values are:
    // 3 Abort pressed
    // 2 Cancel pressed
    // 11 Continue pressed
    // 5 Ignore pressed
    // 7 No pressed
    // 1 OK Pressed
    // 4 Retry pressed
    // 10 Try again pressed
    // 6 Yes pressed

    return result == 1;
}
#elif __linux__
static void GTKCallback(GtkDialog *dialog, gint response_id, gpointer user_data) {
	bool* response = (bool *)(user_data);
	*response = response_id == GTK_RESPONSE_ACCEPT;
	gtk_widget_destroy(GTK_WIDGET(dialog));
	gtk_main_quit();
}

bool linuxPrompt(std::string title, std::string body) {
	GtkWidget *dialog = gtk_dialog_new_with_buttons(
			title.c_str(),
			NULL,
			GTK_DIALOG_MODAL,
			"_OK",
			GTK_RESPONSE_ACCEPT,
			"_Cancel",
			GTK_RESPONSE_REJECT,
			NULL
	);

	GtkWidget *label, *content_area;
	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	label = gtk_label_new(body.c_str());
	gtk_container_add(GTK_CONTAINER(content_area), label);


	bool response;
	g_signal_connect(dialog, "response", G_CALLBACK(GTKCallback), &response);
	gtk_widget_show_all(dialog);
	gtk_main();
	return response;

}
#endif

bool prompt(std::string title, std::string body) {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        return win32Prompt(title, body);
    #elif __linux__
	return linuxPrompt(title, body);
    #endif
}
