#include "folderSelect.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <windows.h>
    #include <shlobj.h>
#elif __linux__
    #include <iostream>
    #include <climits>
    #include <gtk/gtk.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData) {

    if(uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char *) lpData;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string Win32_BrowseFolder(std::string saved_path, std::string title) {
    TCHAR path[MAX_PATH];

    const char * path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle  = (title.c_str());
    bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn       = BrowseCallbackProc;
    bi.lParam     = (LPARAM) path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList ( pidl, path );

        //free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }

        return path;
    }

    return "";
}
#else
static void GTKBrowseFolderCallback(GtkFileChooserDialog *dialog, gint response_id, gpointer user_data) {
	if (response_id == GTK_RESPONSE_ACCEPT) {
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		gchar *filename = gtk_file_chooser_get_filename(chooser);
		std::string *str = (std::string *)(user_data);
		str->assign(filename);
		g_free(filename);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	gtk_main_quit();
}

std::string GTK_BrowseFolder(std::string title) {
	GtkWidget *dialog = gtk_file_chooser_dialog_new(title.c_str(),
			NULL,
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL,
			GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK,
			GTK_RESPONSE_ACCEPT,
			NULL);

	std::string response;
	g_signal_connect(dialog, "response", G_CALLBACK(GTKBrowseFolderCallback), &response);
	gtk_widget_show_all(dialog);
	gtk_main();
	return response;
}
#endif

std::string BrowseFolder(std::string saved_path, std::string title) {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        return Win32_BrowseFolder(saved_path, title);
    #else
	return GTK_BrowseFolder(title);
    #endif
}
