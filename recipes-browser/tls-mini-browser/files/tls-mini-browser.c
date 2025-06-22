/*
* gcc `pkg-config --cflags gtk+-3.0` `pkg-config --cflags webkit2gtk-4.0` tls-mini-browser.c `pkg-config --libs gtk+-3.0` `pkg-config --libs webkit2gtk-4.0` -o tls-mini-browser.out
* ./tls-mini-browser
*/

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static gboolean on_load_failed_with_tls_errors(WebKitWebView *web_view, gchar *failing_uri, GTlsCertificate *certificate, GTlsCertificateFlags errors, gpointer user_data) {
    g_print("TLS error for URI: %s\n", failing_uri);
    g_print("Ignoring TLS errors and continuing...\n");
    return FALSE; // allow WebKit to continue with the load
}

static void on_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data) {
    switch (load_event)
    {
    case WEBKIT_LOAD_STARTED:
        g_print("Load started\n");
        break;
    case WEBKIT_LOAD_REDIRECTED:
        g_print("Load redirected\n");
        break;
    case WEBKIT_LOAD_COMMITTED:
        g_print("Load committed\n");
        break;
    case WEBKIT_LOAD_FINISHED:
        g_print("Load finished successfully\n");
        break;
    }
}

static gboolean on_load_failed(WebKitWebView *web_view, WebKitLoadEvent load_event, gchar *failing_uri, GError *error, gpointer user_data)
{
    g_print("Load failed for URI: %s\n", failing_uri);
    g_print("Error: %s\n", error->message);
    return FALSE;
    // Need to add auto referesh interval
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *webview;
    WebKitWebContext *web_context;
    WebKitWebsiteDataManager *data_manager;

    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_fullscreen(GTK_WINDOW(window));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    data_manager = webkit_website_data_manager_new(NULL);    
    webkit_website_data_manager_set_tls_errors_policy(data_manager, WEBKIT_TLS_ERRORS_POLICY_IGNORE);
    web_context = webkit_web_context_new_with_website_data_manager(data_manager);
    webview = webkit_web_view_new_with_context(web_context);

    // load event handlers for debugging
    g_signal_connect(webview, "load-changed", G_CALLBACK(on_load_changed), NULL);
    g_signal_connect(webview, "load-failed", G_CALLBACK(on_load_failed), NULL);
    g_signal_connect(webview, "load-failed-with-tls-errors", G_CALLBACK(on_load_failed_with_tls_errors), NULL);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), "https://google.com");

    gtk_container_add(GTK_CONTAINER(window), webview);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.minimalbrowser", 0); // use 0 instead of G_APPLICATION_DEFAULT_FLAGS for GTK3 compatibility
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}