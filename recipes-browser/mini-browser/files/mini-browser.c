/*
* gcc `pkg-config --cflags gtk+-3.0` `pkg-config --cflags webkit2gtk-4.0` mini-browser.c `pkg-config --libs gtk+-3.0` `pkg-config --libs webkit2gtk-4.0` -o mini-browser.out
* ./mini-browser.out
*/

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *webview;

    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    
    webview = webkit_web_view_new();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), "file:///home/assets/index.html");
    
    gtk_container_add(GTK_CONTAINER(window), webview);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.minimalbrowser", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}