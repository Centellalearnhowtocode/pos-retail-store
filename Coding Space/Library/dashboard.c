#include <gtk/gtk.h>
#include <stdio.h>

// 1. Define load_css at the top so main can see it
void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, 
        GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Make sure style.css is in the same folder!
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    g_object_unref(provider);
}

GtkWidget* create_numpad() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    const char *labels[] = {
        "1", "2", "3", "Remove Item", "Cashier",
        "4", "5", "6", "QTY", "Free",
        "7", "8", "9", "Price", "Note",
        "0", "Del", "Clear", "Void", "Submit"
    };

    for (int i = 0; i < 20; i++) {
        GtkWidget *btn = gtk_button_new_with_label(labels[i]);
        // Set CSS names for colors
        if (g_strcmp0(labels[i], "Void") == 0) {
            gtk_widget_set_name(btn, "btn-void");
        } else if (g_strcmp0(labels[i], "Submit") == 0) {
            gtk_widget_set_name(btn, "btn-submit");
        } else {
            gtk_widget_set_name(btn, "btn-pos");
        }
        gtk_grid_attach(GTK_GRID(grid), btn, i % 5, i / 5, 1, 1);
    }
    return grid;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // 2. Load CSS before creating the window
    load_css();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 600);
    gtk_window_set_title(GTK_WINDOW(window), "POS Dashboard");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // Header
    GtkWidget *header = gtk_label_new("Date: 2026-04-26 | Shift: Morning | Bill #: 0000");
    gtk_widget_set_name(header, "header-bar");
    gtk_box_pack_start(GTK_BOX(main_vbox), header, FALSE, FALSE, 10);

    GtkWidget *content_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), content_hbox, TRUE, TRUE, 0);

    // Left Side: Invoice Selection
    GtkWidget *left_grid = gtk_grid_new();
    gtk_widget_set_name(left_grid, "left-panel");
    gtk_grid_set_row_spacing(GTK_GRID(left_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(left_grid), 10);
    for(int i=0; i<8; i++) {
        char buf[20]; sprintf(buf, "Invoice %d", (i % 3) + 1);
        GtkWidget *btn = gtk_button_new_with_label(buf);
        gtk_widget_set_size_request(btn, 120, 150);
        gtk_grid_attach(GTK_GRID(left_grid), btn, i % 4, i / 4, 1, 1);
    }
    gtk_box_pack_start(GTK_BOX(content_hbox), left_grid, TRUE, TRUE, 20);

    // Right Side: Checkout
    GtkWidget *right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(right_vbox, "right-panel");
    
    GtkWidget *order_list = gtk_label_new("Item Name 1    1 x $5.00    $5.00\nItem Name 2    2 x $5.00    $10.00");
    gtk_widget_set_name(order_list, "order-area");
    gtk_box_pack_start(GTK_BOX(right_vbox), order_list, TRUE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(right_vbox), create_numpad(), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content_hbox), right_vbox, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}