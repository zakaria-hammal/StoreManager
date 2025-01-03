#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include "dbmanager.h"

void DestroyCategoryList(Clist *C)
{
    EClist *L = C->head;

    while (C->head)
    {
        L = C->head;
        C->head = C->head->next;
        free(L);
    }
    
    C->ElementsNumber = 0;
}

void DestroyProductList(Plist *P)
{
    EPlist *L = P->head;

    while (P->head)
    {
        L = P->head;
        P->head = P->head->next;
        free(L);
    }
    
    P->ElementsNumber = 0;
}

void DestroyUserList(Ulist *U)
{
    EUlist *L;

    while (U->head)
    {
        L = U->head;
        U->head = U->head->next;
        free(L);
    }
    
    U->ElementsNumber = 0;
}

sqlite3 *db;

Clist Categories;
Plist Products;
Ulist Users;
char actual_username[41];

GtkWidget *window;
GtkWidget *stack[3];
GtkWidget *boxUnlogged;
GtkWidget *boxLogged;
GtkWidget *sidebar[2];
GtkWidget *UnloggedBtn[2];
GtkWidget *LoggedBtn[6];
GtkWidget *UnloggedLabel[6];
GtkWidget *UnloggedGrid;
GtkWidget *UnloggedEntry[2];
GtkEntryBuffer *UnloggedEntryBuffer[2];
GtkWidget *AddCategoryGrid;
GtkWidget *AddCategoryButton;
GtkWidget *AddCategoryEntry;
GtkEntryBuffer *AddCategoryEntryBuffer;
GtkWidget *AddCategoryLabel[4];
GtkWidget *AddProductGrid;
GtkWidget *AddUserGrid;
GtkWidget *UsernameLabel;

static void AddCat(GtkWidget *widget, gpointer user_data)
{
    Category cat;

    char *string_categoryname = g_strdup(gtk_entry_buffer_get_text(AddCategoryEntryBuffer));

    strcpy(cat.CategoryName, string_categoryname);

    int status = AddCategorie(db, cat);

    if (status == -1)
    {
        gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "Category Name Already Exists");
        gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "uncorrect");
    }
    else if (status)
    {
        gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "Internal Server Error");
        gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "uncorrect");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "Category Added Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "correct");
    }
    
}

static void GoToLogin(GtkWidget *widget, gpointer user_data)
{
    if(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[1])), "grid_unlogged"))
    {
        gtk_stack_set_visible_child_name(GTK_STACK(stack[1]), "grid_unlogged");
    }
}

static void GoToUnloggedAbout(GtkWidget *widget, gpointer user_data)
{
    if(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[1])), "label_unlogged"))
    {
        gtk_stack_set_visible_child_name(GTK_STACK(stack[1]), "label_unlogged");

        gtk_label_set_label(GTK_LABEL(UnloggedLabel[3]), "");
        gtk_widget_remove_css_class(GTK_WIDGET(UnloggedLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(UnloggedLabel[3]), "uncorrect");
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[0]), "", 0);
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[1]), "", 0);
    }
}

static void Login(GtkWidget *widget, gpointer user_data)
{
    GetUsers(&Users, db);

    char *string_username = g_strdup(gtk_entry_buffer_get_text(UnloggedEntryBuffer[0]));
    char username[41] = "";

    strcpy(username, string_username);

    char *string_password = g_strdup(gtk_entry_buffer_get_text(UnloggedEntryBuffer[1]));
    char password[41] = "";

    strcpy(password, string_password);

    if(LogInUser(Users, username, password))
    {
        gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[3]), "uncorrect");
        gtk_label_set_label(GTK_LABEL(UnloggedLabel[3]), "Username or/and password incorrect");
    }
    else
    {
        gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[3]), "correct");
        gtk_label_set_label(GTK_LABEL(UnloggedLabel[3]), "Welcome");
        strcpy(actual_username, username);
        strcat(actual_username, "\n");
        
        gtk_label_set_label(GTK_LABEL(UsernameLabel), actual_username);

        gtk_stack_set_visible_child_name(GTK_STACK(stack[0]), "box_logged");

        gtk_label_set_label(GTK_LABEL(UnloggedLabel[3]), "");
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[0]), "", 0);
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[1]), "", 0);
    }
    
    DestroyUserList(&Users);
}

static void on_activate(GtkApplication *app)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Store Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 900);

    stack[0] = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack[0]), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack[0]), 300);

    stack[1] = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack[1]), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack[1]), 300);

    stack[2] = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack[2]), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack[2]), 300);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    GFile *cssFile = g_file_new_for_path("styles.css");

    gtk_css_provider_load_from_file(provider, cssFile);

    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    //User is not loggedin

    boxUnlogged = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    sidebar[0] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar[0], 200, 900);

    UnloggedBtn[0] = gtk_button_new_with_label("About");
    g_signal_connect (UnloggedBtn[0], "clicked", G_CALLBACK(GoToUnloggedAbout), NULL);
    UnloggedBtn[1] = gtk_button_new_with_label("Log In");
    g_signal_connect (UnloggedBtn[1], "clicked", G_CALLBACK(GoToLogin), NULL);

    gtk_box_append(GTK_BOX(sidebar[0]), UnloggedBtn[0]);
    gtk_box_append(GTK_BOX(sidebar[0]), UnloggedBtn[1]);

    UnloggedLabel[0] = gtk_label_new_with_mnemonic("This a simple C program that manages a store\n\nCreated By Zakaria Hammal");
    gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[0]), "alone-bold-label");

    gtk_widget_set_hexpand(UnloggedLabel[0], TRUE);
    gtk_widget_set_vexpand(UnloggedLabel[0], TRUE);

    UnloggedGrid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(UnloggedGrid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(UnloggedGrid), 20);

    gtk_grid_set_row_homogeneous(GTK_GRID(UnloggedGrid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(UnloggedGrid), TRUE);

    UnloggedLabel[1] = gtk_label_new_with_mnemonic("Username :");
    gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[1]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UnloggedLabel[1]), 0.0);
    UnloggedLabel[2] = gtk_label_new_with_mnemonic("Password :");
    gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[2]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UnloggedLabel[2]), 0.0);
    UnloggedLabel[3] = gtk_label_new_with_mnemonic("");
    gtk_widget_add_css_class(GTK_WIDGET(UnloggedLabel[3]), "alone-bold-label");
    UnloggedLabel[4] = gtk_label_new_with_mnemonic("");
    UnloggedLabel[5] = gtk_label_new_with_mnemonic("");

    UnloggedEntryBuffer[0] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[0]), 40);

    UnloggedEntry[0] = gtk_entry_new_with_buffer(UnloggedEntryBuffer[0]);

    UnloggedEntryBuffer[1] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[1]), 40);

    UnloggedEntry[1] = gtk_entry_new_with_buffer(UnloggedEntryBuffer[1]);

    UnloggedBtn[2] = gtk_button_new_with_label("Login");
    g_signal_connect (UnloggedBtn[2], "clicked", G_CALLBACK(Login), NULL);

    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedLabel[5]), 0, 0, 12, 1);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedLabel[1]), 1, 1, 10, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedEntry[0]), 1, 3, 8, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedLabel[2]), 1, 5, 10, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedEntry[1]), 1, 7, 8, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedLabel[3]), 0, 9, 12, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedBtn[2]), 5, 11, 5, 2);
    gtk_grid_attach(GTK_GRID(UnloggedGrid), GTK_WIDGET(UnloggedLabel[4]), 0, 13, 12, 1);

    gtk_widget_set_hexpand(stack[1], TRUE);
    gtk_widget_set_vexpand(stack[1], TRUE);

    gtk_stack_add_titled(GTK_STACK(stack[1]), UnloggedGrid, "grid_unlogged", "UnloggedLogin");
    gtk_stack_add_titled(GTK_STACK(stack[1]), UnloggedLabel[0], "label_unlogged", "UnloggedAbout");

    gtk_box_append(GTK_BOX(boxUnlogged), sidebar[0]);
    gtk_box_append(GTK_BOX(boxUnlogged), stack[1]);

    // User is loggedin

    boxLogged = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    sidebar[1] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar[0], 200, 900);

    UsernameLabel = gtk_label_new_with_mnemonic("");
    gtk_widget_add_css_class(GTK_WIDGET(UsernameLabel), "username");

    LoggedBtn[0] = gtk_button_new_with_label("View Categories");
    LoggedBtn[1] = gtk_button_new_with_label("Add Categorie");
    LoggedBtn[2] = gtk_button_new_with_label("View Products");
    LoggedBtn[3] = gtk_button_new_with_label("Add Product");
    LoggedBtn[4] = gtk_button_new_with_label("View Users");
    LoggedBtn[5] = gtk_button_new_with_label("Add User");

    gtk_box_append(GTK_BOX(sidebar[1]), UsernameLabel);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[0]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[1]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[2]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[3]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[4]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[5]);

    AddCategoryGrid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(AddCategoryGrid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(AddCategoryGrid), 20);

    gtk_grid_set_row_homogeneous(GTK_GRID(AddCategoryGrid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(AddCategoryGrid), TRUE);

    AddCategoryLabel[0] = gtk_label_new_with_mnemonic("");
    AddCategoryLabel[1] = gtk_label_new_with_mnemonic("Category Name :");
    gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[1]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddCategoryLabel[1]), 0.0);
    AddCategoryLabel[2] = gtk_label_new_with_mnemonic("");
    gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "alone-bold-label");
    AddCategoryButton = gtk_button_new_with_label("Add Category");
    g_signal_connect (AddCategoryButton, "clicked", G_CALLBACK(AddCat), NULL);
    AddCategoryLabel[3] = gtk_label_new_with_mnemonic("");

    AddCategoryEntryBuffer = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddCategoryEntryBuffer), 40);

    AddCategoryEntry = gtk_entry_new_with_buffer(AddCategoryEntryBuffer);

    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryLabel[0]), 0, 0, 12, 1);
    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryLabel[1]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryEntry), 1, 2, 8, 1);
    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryLabel[2]), 1, 3, 12, 2);
    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryButton), 5, 5, 6, 1);
    gtk_grid_attach(GTK_GRID(AddCategoryGrid), GTK_WIDGET(AddCategoryLabel[3]), 0, 6, 12, 1);

    gtk_stack_add_titled(GTK_STACK(stack[2]), AddCategoryGrid, "grid_add_category", "AddCategory");

    gtk_widget_set_hexpand(stack[2], TRUE);
    gtk_widget_set_vexpand(stack[2], TRUE);

    gtk_box_append(GTK_BOX(boxLogged), sidebar[1]);
    gtk_box_append(GTK_BOX(boxLogged), stack[2]);

    gtk_stack_add_titled(GTK_STACK(stack[0]), boxUnlogged, "box_unlogged", "Unlogged");
    gtk_stack_add_titled(GTK_STACK(stack[0]), boxLogged, "box_logged", "Logged");

    gtk_window_set_child(GTK_WINDOW(window), stack[0]);

    gtk_window_present(GTK_WINDOW(window));
}

static void on_app_shutdown(GApplication *app, gpointer user_data) 
{
    DestroyCategoryList(&Categories);
    DestroyProductList(&Products);
    DestroyUserList(&Users);
}

int main(int argc, char* argv[])
{
    strcpy(actual_username, "");

    if (sqlite3_open("store.db", &db))
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    Categories.ElementsNumber = 0;
    Categories.head = NULL;

    Products.ElementsNumber = 0;
    Products.head = NULL;

    Users.ElementsNumber = 0;
    Users.head = NULL;    

    GtkApplication *app;
    int status;

    app = gtk_application_new ("stackof.holger.entry", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_app_shutdown), NULL);

    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}