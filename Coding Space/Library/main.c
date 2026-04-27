#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAX_RATE 0.10
#define START_INVOICE_NO 1001

#define WINDOW_W 810
#define WINDOW_H 455
#define RIGHT_X 535
#define RIGHT_W 275

typedef struct {
    const char *name;
    const char *subtitle;
    double price;
} Product;

typedef struct {
    const char *id;
    const char *name;
} Staff;

typedef struct {
    const Product *product;
    int qty;
} CartLine;

static Product products[] = {
    {"Coffee", "Drink", 1.50},
    {"Tea", "Drink", 1.25},
    {"Water", "Bottle", 0.75},
    {"Soda", "Drink", 1.10},
    {"Bread", "Food", 2.00},
    {"Milk", "Dairy", 2.50},
    {"Rice 1kg", "Grocery", 3.75},
    {"Eggs", "Pack", 4.20}
};

static Staff staff_list[] = {
    {"1001", "Soksan"},
    {"1002", "Chornay"},
    {"admin", "Rith"}
};

typedef struct {
    GtkWidget *window;
    GtkWidget *stack;

    GtkWidget *staff_entry;
    GtkWidget *login_status_label;

    GtkWidget *date_label;
    GtkWidget *shift_label;
    GtkWidget *bill_label;
    GtkWidget *bill_date_label;
    GtkWidget *cashier_label;

    GtkWidget *product_buttons[G_N_ELEMENTS(products)];
    GtkWidget *order_stack;
    GtkWidget *order_list_box;
    GtkWidget *receipt_text;

    GtkWidget *subtotal_value_label;
    GtkWidget *total_value_label;
    GtkWidget *discount_value_label;

    GtkWidget *selected_line_label;

    CartLine cart[64];
    int cart_count;
    int selected_product;
    guint invoice_no;
    char current_staff_id[64];
    char current_staff_name[128];
} AppData;

static void add_class(GtkWidget *widget, const char *class_name) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, class_name);
}

static GtkWidget *make_label(const char *text, int xalign) {
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), xalign ? 1.0f : 0.0f);
    return label;
}

static GtkWidget *make_fixed_panel(const char *class_name, int width, int height) {
    GtkWidget *box = gtk_event_box_new();
    add_class(box, class_name);
    gtk_widget_set_size_request(box, width, height);
    return box;
}

static const Staff *find_staff_by_id(const char *id) {
    for (guint i = 0; i < G_N_ELEMENTS(staff_list); i++) {
        if (g_strcmp0(staff_list[i].id, id) == 0) {
            return &staff_list[i];
        }
    }
    return NULL;
}

static void update_clock_labels(AppData *app) {
    GDateTime *now = g_date_time_new_now_local();
    char *date_text = g_date_time_format(now, "%Y-%m-%d  %H:%M");
    char *short_text = g_date_time_format(now, "%Y-%m-%d");

    gtk_label_set_text(GTK_LABEL(app->date_label), date_text);
    gtk_label_set_text(GTK_LABEL(app->bill_date_label), short_text);

    g_free(date_text);
    g_free(short_text);
    g_date_time_unref(now);
}

static gboolean clock_timer(gpointer user_data) {
    update_clock_labels((AppData *)user_data);
    return TRUE;
}

static double cart_subtotal(AppData *app) {
    double subtotal = 0.0;
    for (int i = 0; i < app->cart_count; i++) {
        subtotal += app->cart[i].product->price * app->cart[i].qty;
    }
    return subtotal;
}

static void set_money_label(GtkWidget *label, double amount) {
    char buf[64];
    snprintf(buf, sizeof(buf), "$%.2f", amount);
    gtk_label_set_text(GTK_LABEL(label), buf);
}

static void update_totals(AppData *app) {
    double subtotal = cart_subtotal(app);
    double total = subtotal + (subtotal * TAX_RATE);
    set_money_label(app->subtotal_value_label, subtotal);
    set_money_label(app->total_value_label, total);
    gtk_label_set_text(GTK_LABEL(app->discount_value_label), "$0.00");
}

static void update_header(AppData *app) {
    char bill_text[64];
    snprintf(bill_text, sizeof(bill_text), "#%u", app->invoice_no);
    gtk_label_set_text(GTK_LABEL(app->bill_label), bill_text);

    char cashier_text[160];
    snprintf(cashier_text, sizeof(cashier_text), "%s", app->current_staff_name[0] ? app->current_staff_name : "-");
    gtk_label_set_text(GTK_LABEL(app->cashier_label), cashier_text);

    gtk_label_set_text(GTK_LABEL(app->shift_label), "Morning");
}

static void clear_container(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

static void select_product(AppData *app, int index) {
    app->selected_product = index;
    for (guint i = 0; i < G_N_ELEMENTS(products); i++) {
        GtkStyleContext *ctx = gtk_widget_get_style_context(app->product_buttons[i]);
        if ((int)i == index) {
            gtk_style_context_add_class(ctx, "product-selected");
        } else {
            gtk_style_context_remove_class(ctx, "product-selected");
        }
    }

    char text[128];
    snprintf(text, sizeof(text), "%s  $%.2f", products[index].name, products[index].price);
    gtk_label_set_text(GTK_LABEL(app->selected_line_label), text);
}

static GtkWidget *make_order_row(AppData *app, int index) {
    CartLine *line = &app->cart[index];
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(row, RIGHT_W - 18, 39);
    add_class(row, "order-row");

    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(left, 10);
    gtk_widget_set_margin_top(left, 3);
    gtk_widget_set_margin_bottom(left, 3);
    gtk_box_pack_start(GTK_BOX(row), left, TRUE, TRUE, 0);

    GtkWidget *name = make_label(line->product->name, 0);
    add_class(name, "order-name");
    gtk_box_pack_start(GTK_BOX(left), name, FALSE, FALSE, 0);

    GtkWidget *discount = make_label("Discount: %0.00", 0);
    add_class(discount, "order-discount");
    gtk_box_pack_start(GTK_BOX(left), discount, FALSE, FALSE, 0);

    char qty_price[64];
    snprintf(qty_price, sizeof(qty_price), "%d × $%.2f", line->qty, line->product->price);
    GtkWidget *mid = gtk_label_new(qty_price);
    add_class(mid, "order-name");
    gtk_widget_set_size_request(mid, 76, -1);
    gtk_box_pack_start(GTK_BOX(row), mid, FALSE, FALSE, 0);

    char total[64];
    snprintf(total, sizeof(total), "$%.2f", line->qty * line->product->price);
    GtkWidget *right = gtk_label_new(total);
    gtk_label_set_xalign(GTK_LABEL(right), 1.0f);
    add_class(right, "order-name");
    gtk_widget_set_size_request(right, 65, -1);
    gtk_widget_set_margin_end(right, 10);
    gtk_box_pack_start(GTK_BOX(row), right, FALSE, FALSE, 0);

    return row;
}

static void refresh_order_list(AppData *app) {
    clear_container(app->order_list_box);

    GtkWidget *order = make_label("Order: #0000 by Centella", 0);
    add_class(order, "receipt-heading");
    gtk_widget_set_margin_start(order, 10);
    gtk_widget_set_margin_top(order, 8);
    gtk_box_pack_start(GTK_BOX(app->order_list_box), order, FALSE, FALSE, 0);

    for (int i = 0; i < app->cart_count; i++) {
        GtkWidget *row = make_order_row(app, i);
        gtk_box_pack_start(GTK_BOX(app->order_list_box), row, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(app->order_list_box);
    update_totals(app);
}

static void add_product_to_cart(AppData *app, int product_index) {
    if (product_index < 0 || product_index >= (int)G_N_ELEMENTS(products)) {
        return;
    }

    const Product *p = &products[product_index];
    for (int i = 0; i < app->cart_count; i++) {
        if (app->cart[i].product == p) {
            app->cart[i].qty++;
            refresh_order_list(app);
            gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "order");
            return;
        }
    }

    if (app->cart_count < (int)G_N_ELEMENTS(app->cart)) {
        app->cart[app->cart_count].product = p;
        app->cart[app->cart_count].qty = 1;
        app->cart_count++;
    }

    refresh_order_list(app);
    gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "order");
}

static void on_product_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "product-index"));
    select_product(app, index);
    add_product_to_cart(app, index);
}

static char *build_receipt(AppData *app) {
    double subtotal = cart_subtotal(app);
    double tax = subtotal * TAX_RATE;
    double total = subtotal + tax;

    GString *s = g_string_new(NULL);
    GDateTime *now = g_date_time_new_now_local();
    char *time_text = g_date_time_format(now, "%Y-%m-%d %H:%M:%S");

    g_string_append(s, "        RETAIL STORE POS\n");
    g_string_append(s, "================================\n");
    g_string_append_printf(s, "Receipt No : #%u\n", app->invoice_no);
    g_string_append_printf(s, "Date       : %s\n", time_text);
    g_string_append_printf(s, "Cashier    : %s\n", app->current_staff_name);
    g_string_append(s, "--------------------------------\n");
    g_string_append_printf(s, "%-14s %3s %10s\n", "Item", "Qty", "Amount");
    g_string_append(s, "--------------------------------\n");

    for (int i = 0; i < app->cart_count; i++) {
        CartLine *line = &app->cart[i];
        g_string_append_printf(
            s,
            "%-14.14s %3d %10.2f\n",
            line->product->name,
            line->qty,
            line->qty * line->product->price
        );
    }

    g_string_append(s, "--------------------------------\n");
    g_string_append_printf(s, "Subtotal        $%10.2f\n", subtotal);
    g_string_append_printf(s, "Tax 10%%         $%10.2f\n", tax);
    g_string_append_printf(s, "TOTAL           $%10.2f\n", total);
    g_string_append(s, "================================\n");
    g_string_append(s, "   Thank you. Please come again.\n");

    g_free(time_text);
    g_date_time_unref(now);
    return g_string_free(s, FALSE);
}

static void show_receipt_inside_ui(AppData *app) {
    if (app->cart_count == 0) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->receipt_text));
        gtk_text_buffer_set_text(buffer, "No items in invoice.\nAdd an item first.", -1);
        gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "receipt");
        return;
    }

    char *receipt = build_receipt(app);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->receipt_text));
    gtk_text_buffer_set_text(buffer, receipt, -1);
    gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "receipt");
    g_free(receipt);

    app->cart_count = 0;
    app->invoice_no++;
    refresh_order_list(app);
    update_header(app);
}

static void clear_invoice(AppData *app) {
    app->cart_count = 0;
    refresh_order_list(app);
    gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "order");
}

static void on_function_button(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    const char *action = (const char *)g_object_get_data(G_OBJECT(button), "action");

    if (g_strcmp0(action, "Submit") == 0) {
        show_receipt_inside_ui(app);
    } else if (g_strcmp0(action, "Clear") == 0 || g_strcmp0(action, "Void") == 0) {
        clear_invoice(app);
    } else if (g_strcmp0(action, "Cashier") == 0) {
        clear_invoice(app);
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "login");
        gtk_widget_grab_focus(app->staff_entry);
    }
}

static void on_login_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    AppData *app = (AppData *)user_data;
    char id[64];
    g_strlcpy(id, gtk_entry_get_text(GTK_ENTRY(app->staff_entry)), sizeof(id));
    g_strstrip(id);

    if (id[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(app->login_status_label), "Please enter Staff ID.");
        return;
    }

    const Staff *staff = find_staff_by_id(id);
    if (staff == NULL) {
        gtk_label_set_text(GTK_LABEL(app->login_status_label), "Wrong Staff ID. Try 1001, 1002, or admin.");
        return;
    }

    g_strlcpy(app->current_staff_id, staff->id, sizeof(app->current_staff_id));
    g_strlcpy(app->current_staff_name, staff->name, sizeof(app->current_staff_name));
    gtk_entry_set_text(GTK_ENTRY(app->staff_entry), "");
    gtk_label_set_text(GTK_LABEL(app->login_status_label), "");
    update_clock_labels(app);
    update_header(app);
    clear_invoice(app);
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "pos");
}

static void on_login_exit_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    AppData *app = (AppData *)user_data;
    gtk_widget_destroy(app->window);
}

static GtkWidget *create_login_screen(AppData *app) {
    GtkWidget *fixed = gtk_fixed_new();
    add_class(fixed, "app-bg");

    GtkWidget *card = make_fixed_panel("login-card", 206, 151);
    gtk_fixed_put(GTK_FIXED(fixed), card, 285, 181);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 7);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 16);
    gtk_widget_set_margin_start(box, 13);
    gtk_widget_set_margin_end(box, 13);
    gtk_container_add(GTK_CONTAINER(card), box);

    GtkWidget *title = make_label("Retail Store POS", 0);
    add_class(title, "login-title");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

    app->staff_entry = gtk_entry_new();
    add_class(app->staff_entry, "cream-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->staff_entry), "");
    gtk_box_pack_start(GTK_BOX(box), app->staff_entry, FALSE, FALSE, 0);

    GtkWidget *staff_label = make_label("Staff ID", 0);
    add_class(staff_label, "small-login-label");
    gtk_box_pack_start(GTK_BOX(box), staff_label, FALSE, FALSE, 0);

    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), row, FALSE, FALSE, 0);

    GtkWidget *login_button = gtk_button_new_with_label("Log in");
    add_class(login_button, "green-button");
    gtk_box_pack_start(GTK_BOX(row), login_button, TRUE, TRUE, 0);

    GtkWidget *exit_button = gtk_button_new_with_label("Log out");
    add_class(exit_button, "red-button");
    gtk_box_pack_start(GTK_BOX(row), exit_button, TRUE, TRUE, 0);

    app->login_status_label = make_label("Use Staff ID: 1001, 1002, or admin", 0);
    add_class(app->login_status_label, "small-login-label");
    gtk_box_pack_start(GTK_BOX(box), app->login_status_label, FALSE, FALSE, 0);

    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_clicked), app);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_login_exit_clicked), app);
    g_signal_connect(app->staff_entry, "activate", G_CALLBACK(on_login_clicked), app);

    return fixed;
}

static GtkWidget *create_product_card(AppData *app, int index) {
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_size_request(button, 96, 114);
    add_class(button, "product-card");
    g_object_set_data(G_OBJECT(button), "product-index", GINT_TO_POINTER(index));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(button), box);

    GtkWidget *photo = gtk_label_new("Add to cart");
    add_class(photo, "product-photo");
    gtk_box_pack_start(GTK_BOX(box), photo, FALSE, FALSE, 0);

    GtkWidget *sub = gtk_label_new(products[index].subtitle);
    add_class(sub, "product-sub");
    gtk_box_pack_start(GTK_BOX(box), sub, FALSE, FALSE, 0);

    GtkWidget *space = gtk_label_new(" ");
    gtk_box_pack_start(GTK_BOX(box), space, FALSE, FALSE, 7);

    char name_price[96];
    snprintf(name_price, sizeof(name_price), "%s  $%.2f", products[index].name, products[index].price);
    GtkWidget *name = gtk_label_new(name_price);
    add_class(name, "product-price");
    gtk_box_pack_start(GTK_BOX(box), name, FALSE, FALSE, 0);

    g_signal_connect(button, "clicked", G_CALLBACK(on_product_clicked), app);
    app->product_buttons[index] = button;
    return button;
}

static GtkWidget *make_category_button(const char *text) {
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_widget_set_size_request(button, 96, 43);
    add_class(button, "category-button");
    return button;
}

static GtkWidget *make_small_button(AppData *app, const char *text, const char *css, int w, int h) {
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_widget_set_size_request(button, w, h);
    add_class(button, css);
    g_object_set_data_full(G_OBJECT(button), "action", g_strdup(text), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_function_button), app);
    return button;
}

static GtkWidget *create_keypad(AppData *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
    gtk_widget_set_margin_top(grid, 3);
    gtk_widget_set_margin_start(grid, 4);
    gtk_widget_set_margin_end(grid, 4);

    const char *labels[4][5] = {
        {"1", "2", "3", "Remove Item", "Cashier"},
        {"4", "5", "6", "QTY", "Free"},
        {"7", "8", "9", "Price", "Note"},
        {"0", "Del", "Clear", "Void", "Submit"}
    };

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 5; c++) {
            int w = (c < 3) ? 38 : 67;
            const char *css = "key-button";
            if (g_strcmp0(labels[r][c], "Void") == 0) css = "key-red";
            if (g_strcmp0(labels[r][c], "Submit") == 0) css = "key-green";
            GtkWidget *button = make_small_button(app, labels[r][c], css, w, 19);
            gtk_grid_attach(GTK_GRID(grid), button, c, r, 1, 1);
        }
    }

    return grid;
}

static GtkWidget *create_totals_panel(AppData *app) {
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_set_size_request(fixed, RIGHT_W, 34);
    add_class(fixed, "totals-bg");

    GtkWidget *l1 = gtk_label_new("Total order:");
    add_class(l1, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), l1, 8, 3);
    GtkWidget *l2 = gtk_label_new("Subtotal");
    add_class(l2, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), l2, 8, 13);
    GtkWidget *l3 = gtk_label_new("Total");
    add_class(l3, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), l3, 8, 23);

    app->discount_value_label = gtk_label_new("$0.00");
    add_class(app->discount_value_label, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), app->discount_value_label, 174, 3);

    app->subtotal_value_label = gtk_label_new("$0.00");
    add_class(app->subtotal_value_label, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), app->subtotal_value_label, 174, 13);

    app->total_value_label = gtk_label_new("$0.00");
    add_class(app->total_value_label, "tiny-text");
    gtk_fixed_put(GTK_FIXED(fixed), app->total_value_label, 174, 23);

    GtkWidget *discount = gtk_label_new("Discount\nOn Item");
    add_class(discount, "key-button");
    gtk_widget_set_size_request(discount, 75, 34);
    gtk_fixed_put(GTK_FIXED(fixed), discount, 199, 0);

    return fixed;
}

static GtkWidget *create_right_panel(AppData *app) {
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_set_size_request(fixed, RIGHT_W, WINDOW_H - 49);

    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    add_class(top, "receipt-bg");
    gtk_widget_set_size_request(top, RIGHT_W, 278);
    gtk_fixed_put(GTK_FIXED(fixed), top, 0, 0);

    app->order_stack = gtk_stack_new();
    gtk_widget_set_hexpand(app->order_stack, TRUE);
    gtk_widget_set_vexpand(app->order_stack, TRUE);
    gtk_box_pack_start(GTK_BOX(top), app->order_stack, TRUE, TRUE, 0);

    GtkWidget *order_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(order_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    add_class(order_scroll, "receipt-bg");
    app->order_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    add_class(app->order_list_box, "receipt-bg");
    gtk_container_add(GTK_CONTAINER(order_scroll), app->order_list_box);
    gtk_stack_add_named(GTK_STACK(app->order_stack), order_scroll, "order");

    GtkWidget *receipt_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(receipt_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    add_class(receipt_scroll, "receipt-bg");
    app->receipt_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->receipt_text), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->receipt_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->receipt_text), GTK_WRAP_NONE);
    add_class(app->receipt_text, "receipt-text");
    gtk_container_add(GTK_CONTAINER(receipt_scroll), app->receipt_text);
    gtk_stack_add_named(GTK_STACK(app->order_stack), receipt_scroll, "receipt");

    GtkWidget *totals = create_totals_panel(app);
    gtk_fixed_put(GTK_FIXED(fixed), totals, 0, 280);

    GtkWidget *keypad = create_keypad(app);
    gtk_fixed_put(GTK_FIXED(fixed), keypad, 0, 315);

    gtk_stack_set_visible_child_name(GTK_STACK(app->order_stack), "order");
    return fixed;
}

static GtkWidget *create_header(AppData *app) {
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_set_size_request(fixed, WINDOW_W, 49);
    add_class(fixed, "top-bar");

    GtkWidget *date_text = gtk_label_new("Date       :");
    add_class(date_text, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), date_text, 28, 9);

    app->date_label = gtk_label_new("");
    add_class(app->date_label, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), app->date_label, 88, 9);

    GtkWidget *shift_text = gtk_label_new("Shift      :");
    add_class(shift_text, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), shift_text, 28, 25);

    app->shift_label = gtk_label_new("Morning");
    add_class(app->shift_label, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), app->shift_label, 88, 25);

    GtkWidget *bill_text = gtk_label_new("Bill #     :");
    add_class(bill_text, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), bill_text, 548, 9);

    app->bill_label = gtk_label_new("#1001");
    add_class(app->bill_label, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), app->bill_label, 610, 9);

    GtkWidget *bd_text = gtk_label_new("Date       :");
    add_class(bd_text, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), bd_text, 548, 22);

    app->bill_date_label = gtk_label_new("");
    add_class(app->bill_date_label, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), app->bill_date_label, 610, 22);

    GtkWidget *cash_text = gtk_label_new("Cashier    :");
    add_class(cash_text, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), cash_text, 548, 35);

    app->cashier_label = gtk_label_new("-");
    add_class(app->cashier_label, "header-small");
    gtk_fixed_put(GTK_FIXED(fixed), app->cashier_label, 610, 35);

    return fixed;
}

static GtkWidget *create_pos_screen(AppData *app) {
    GtkWidget *fixed = gtk_fixed_new();
    add_class(fixed, "app-bg");

    GtkWidget *header = create_header(app);
    gtk_fixed_put(GTK_FIXED(fixed), header, 0, 0);

    GtkWidget *products_bg = make_fixed_panel("product-area", RIGHT_X, 358);
    gtk_fixed_put(GTK_FIXED(fixed), products_bg, 0, 49);

    int x_positions[] = {26, 152, 278, 404};
    int y_positions[] = {59, 183};
    int index = 0;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 4; c++) {
            GtkWidget *card = create_product_card(app, index);
            gtk_fixed_put(GTK_FIXED(fixed), card, x_positions[c], y_positions[r]);
            index++;
        }
    }

    app->selected_line_label = gtk_label_new("Select an item");
    add_class(app->selected_line_label, "selected-item");
    gtk_fixed_put(GTK_FIXED(fixed), app->selected_line_label, 26, 320);

    GtkWidget *cat_bg = make_fixed_panel("bottom-bar", RIGHT_X, 48);
    gtk_fixed_put(GTK_FIXED(fixed), cat_bg, 0, 407);

    GtkWidget *cat1 = make_category_button("Category");
    GtkWidget *cat2 = make_category_button("Category");
    GtkWidget *cat3 = make_category_button("Category");
    GtkWidget *cat4 = make_category_button("Category");
    gtk_fixed_put(GTK_FIXED(fixed), cat1, 26, 409);
    gtk_fixed_put(GTK_FIXED(fixed), cat2, 152, 409);
    gtk_fixed_put(GTK_FIXED(fixed), cat3, 278, 409);
    gtk_fixed_put(GTK_FIXED(fixed), cat4, 404, 409);

    GtkWidget *right = create_right_panel(app);
    gtk_fixed_put(GTK_FIXED(fixed), right, RIGHT_X, 49);

    select_product(app, 0);
    refresh_order_list(app);
    return fixed;
}

static void load_css(void) {
    const char *css =
        "window { background-color: #669bbcff; }"
        ".app-bg { background-color: #383b52; }"
        ".top-bar { background-color: #00384f; }"
        ".header-small { color: #130127; font-size: 10px; }"
        ".product-area { background-color: #d7d7d7; }"
        ".bottom-bar { background-color: #00384f; }"
        ".product-card { background-image:url('Retail.jpg') ; background-size: cover; background-position: center; background-color: #ffffff; border: 0; border-radius: 13px; color: #00384f; }"
        ".product-card:hover { background-color: #f9f9f9; }"
        ".product-selected { background-color: #c7c66a; }"
        ".product-photo { color: #00384f; font-size: 16px; }"
        ".product-sub { color: #00384f; font-size: 10px; font-weight: bold; }"
        ".product-price {color: #21d648;font-size: 11px;  font-weight: bold; }"
        ".selected-item { color: #00384f; font-size: 10px; }"
        ".category-button { background-image: none; background-color: #ffffff; color: #00384f; border: 0; border-radius: 0; font-size: 10px; }"
        ".receipt-bg { background-color: #fff1cf; }"
        ".receipt-heading { color: #00384f; font-size: 10px; }"
        ".order-row { background-color: #fffaf0; border-top: 1px solid #ebd5ab; }"
        ".order-name { color: #00384f; font-size: 10px; }"
        ".order-discount { color: #00384f; font-size: 6px; }"
        ".totals-bg { background-color: #fffaf0; border-top: 4px solid #00384f; }"
        ".tiny-text { color: #00384f; font-size: 7px; }"
        ".key-button { background-image: none; background-color: #ffffff; color: #00384f; border: 0; border-radius: 0; font-size: 9px; }"
        ".key-button:hover { background-color: #edf6fb; }"
        ".key-red { background-image: none; background-color: #df0617; color: #00384f; border: 0; border-radius: 0; font-size: 9px; }"
        ".key-green { background-image: none; background-color: #63be63; color: #ffffff; border: 0; border-radius: 0; font-size: 9px; }"
        ".receipt-text { background-color: #fff1cf; color: #00384f; font-family: monospace; font-size: 10px; padding: 8px; }"
        ".login-card { background-color: #00384f; border-radius: 14px; }"
        ".login-title { color: #72aeca; font-size: 10px; font-weight: bold; }"
        ".small-login-label { color: #8fc2dc; font-size: 6px; }"
        ".cream-entry { background-image: none; background-color: #fff2d2; color: #000000; border-radius: 6px; border: 0; padding: 10px; }"
        ".green-button { background-image: none; background-color: #63be63; color: #000000; border-radius: 3px; border: 0; font-size: 10px; }"
        ".red-button { background-image: none; background-color: #df0617; color: #000000; border-radius: 3px; border: 0; font-size: 10px; }";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    load_css();

    AppData app;
    memset(&app, 0, sizeof(AppData));
    app.invoice_no = START_INVOICE_NO;
    app.selected_product = 0;

    app.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app.window), "Retail Store POS");
    gtk_window_set_default_size(GTK_WINDOW(app.window), WINDOW_W, WINDOW_H);
    gtk_window_set_resizable(GTK_WINDOW(app.window), FALSE);
    gtk_window_set_position(GTK_WINDOW(app.window), GTK_WIN_POS_CENTER);
    g_signal_connect(app.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    app.stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app.stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(app.stack), 180);
    gtk_container_add(GTK_CONTAINER(app.window), app.stack);

    GtkWidget *login_screen = create_login_screen(&app);
    GtkWidget *pos_screen = create_pos_screen(&app);

    gtk_stack_add_named(GTK_STACK(app.stack), login_screen, "login");
    gtk_stack_add_named(GTK_STACK(app.stack), pos_screen, "pos");
    gtk_stack_set_visible_child_name(GTK_STACK(app.stack), "login");

    g_timeout_add_seconds(30, clock_timer, &app);
    update_clock_labels(&app);
    update_header(&app);

    gtk_widget_show_all(app.window);
    gtk_widget_grab_focus(app.staff_entry);
    gtk_main();
    return 0;
}
