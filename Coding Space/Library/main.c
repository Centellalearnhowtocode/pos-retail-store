#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>

static void activate(GtkApplication* app, gpointer user_data) {

    GtkWidget *window, *main_box, *button_row, *title, *input_field, *login_btn, *logout_btn;


    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "POS Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 200);

    // Big Container
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 25);
    // The Title
    title = gtk_label_new("Retail Store POS");
    gtk_widget_set_name(title, "pos-title");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE,0);

    // The Input Field
    input_field = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(input_field), FALSE);
    gtk_widget_set_name(input_field, "pos-input");
    gtk_box_pack_start(GTK_BOX(main_box), input_field, FALSE, FALSE,0);

    // Button 
    button_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_row, FALSE, FALSE,0);

    login_btn = gtk_button_new_with_label("Login");
    gtk_widget_set_name(login_btn, "btn-login");
    gtk_box_pack_start(GTK_BOX(button_row), login_btn, TRUE, TRUE,0);

    logout_btn = gtk_button_new_with_label("Log out");
    gtk_widget_set_name(logout_btn, "btn-logout");
    gtk_box_pack_start(GTK_BOX(button_row), logout_btn, TRUE, TRUE,0);
    /// Link toCSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(provider);

    // Show window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.pos.scratch", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}