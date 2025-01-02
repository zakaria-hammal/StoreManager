#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include "dbmanager.h"

Clist Categories;
Plist Products;
Ulist Users;

GtkWidget* window;
GtkWidget* gridUnlogged;

static void on_activate(GtkApplication *app) 
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Store Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 900);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar, 200, 900);

    GtkWidget *btn1 = gtk_button_new_with_label("Home");
    GtkWidget *btn2 = gtk_button_new_with_label("Settings");
    GtkWidget *btn3 = gtk_button_new_with_label("About");
    gtk_box_append(GTK_BOX(sidebar), btn1);
    gtk_box_append(GTK_BOX(sidebar), btn2);
    gtk_box_append(GTK_BOX(sidebar), btn3);

    GtkWidget *content = gtk_label_new("Main Content Area");
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);

    gtk_box_append(GTK_BOX(hbox), sidebar);
    gtk_box_append(GTK_BOX(hbox), content);

    gtk_window_set_child(GTK_WINDOW(window), hbox);

    gtk_window_present(GTK_WINDOW(window));
}

static void on_app_shutdown(GApplication *app, gpointer user_data) 
{

}

int main(int argc, char* argv[])
{
    sqlite3 *db;

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