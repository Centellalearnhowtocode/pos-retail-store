    // The Title
    title = gtk_label_new("Retail Store POS");
    gtk_widget_set_name(title, "pos-title");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);

    // The Input Field
    input_field = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(input_field), FALSE);
    gtk_widget_set_name(input_field, "pos-input");
    gtk_box_pack_start(GTK_BOX(main_box), input_field, FALSE, FALSE, 0);

    // Button 
    button_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_row, FALSE, FALSE, 0);

    login_btn = gtk_button_new_with_label("Login");
    gtk_widget_set_name(login_btn, "btn-login");
    gtk_box_pack_start(GTK_BOX(button_row), login_btn, TRUE, TRUE, 0);

    logout_btn = gtk_button_new_with_label("Log out");
    gtk_widget_set_name(logout_btn, "btn-logout");
    gtk_box_pack_start(GTK_BOX(button_row), logout_btn, TRUE, TRUE, 0);