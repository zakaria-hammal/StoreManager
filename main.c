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
GtkWidget *scrolled_window[3];
GtkWidget *stack[3];
GtkWidget *boxUnlogged;
GtkWidget *boxLogged;
GtkWidget *sidebar[2];
GtkWidget *UnloggedBtn[2];
GtkWidget *LoggedBtn[9];
GtkWidget *UnloggedLabel[6];
GtkWidget *LoggedLabel;
GtkWidget *UnloggedGrid;
GtkWidget *UnloggedEntry[2];
GtkEntryBuffer *UnloggedEntryBuffer[2];
GtkWidget *AddCategoryGrid;
GtkWidget *AddCategoryButton;
GtkWidget *AddCategoryEntry;
GtkEntryBuffer *AddCategoryEntryBuffer;
GtkWidget *AddCategoryLabel[4];
GtkWidget *AddUserGrid;
GtkWidget *UsernameLabel;
GtkWidget *ViewCategoriesBox;
GtkWidget *ViewCategoriesHead;
GtkWidget *ViewCategoriesFoot;
GtkWidget **ViewCategoriesLabel;
GtkWidget **NoCategoryLabel;
GtkWidget **DeleteCategoriesButton;
GtkWidget **ViewCategoriesSubBoxes;
GtkWidget *AddProductGrid;
GtkWidget *ViewProductsBox;
GtkWidget *ViewProductsHead;
GtkWidget *ViewProductsFoot;
GtkWidget **ViewProductsLabel;
GtkWidget **NoProductLabel;
GtkWidget **DeleteProductsButton;
GtkWidget **ViewProductsSubBoxes;
GtkWidget *Dropdown;
GtkWidget *AddProductLabel[6];
GtkWidget *AddProductButton;
GtkWidget *AddProductEntry[4];
GtkEntryBuffer *AddProductEntryBuffer[4];
GtkWidget *BlankBox;
GListStore *store;
GtkWidget *BlankLabel;
GtkWidget *UpdateProductGrid;
GListStore *UpdateStore;
GtkWidget *UpdateProductDropdown;
GtkWidget *UpdateProductLabel[5];
GtkWidget *UpdateProductEntry[2];
GtkEntryBuffer *UpdateProductEntryBuffer[2];
GtkWidget *UpdateProductButton[2];
GtkWidget *AddUserGrid;
GtkWidget *ViewUsersBox;
GtkWidget *ViewUsersHead;
GtkWidget *ViewUsersFoot;
GtkWidget **ViewUsersLabel;
GtkWidget **NoUserLabel;
GtkWidget **ViewUsersSubBoxes;
GtkWidget *Dropdown;
GtkWidget *AddUserLabel[3];
GtkWidget *AddUserButton;
GtkWidget *AddUserEntry[2];
GtkEntryBuffer *AddUserEntryBuffer[2];


int n;
int n1;
int n2;

static void DeleteCat(GtkWidget *widget, gpointer user_data);
static void DeletePro(GtkWidget *widget, gpointer user_data);

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
        gtk_widget_remove_css_class(GTK_WIDGET(AddCategoryLabel[2]), "correct");
    }
    else if (status)
    {
        gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "Internal Server Error");
        gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "uncorrect");
        gtk_widget_remove_css_class(GTK_WIDGET(AddCategoryLabel[2]), "correct");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "Category Added Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(AddCategoryLabel[2]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(AddCategoryLabel[2]), "uncorrect");
    }
    
}

static void AddPro(GtkWidget *widget, gpointer user_data)
{
    Product pro;

    char *string_productname = g_strdup(gtk_entry_buffer_get_text(AddProductEntryBuffer[0]));

    strcpy(pro.ProductName, string_productname);

    char *string_desc = g_strdup(gtk_entry_buffer_get_text(AddProductEntryBuffer[1]));

    strcpy(pro.Description, string_desc);

    char *string_stock = g_strdup(gtk_entry_buffer_get_text(AddProductEntryBuffer[2]));

    pro.Stock = atoi(string_stock);

    char *string_price = g_strdup(gtk_entry_buffer_get_text(AddProductEntryBuffer[3]));

    pro.Price = strtod(string_price, NULL);

    GtkStringObject *string_cat = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(Dropdown));

    strcpy(pro.CategoryName, gtk_string_object_get_string(string_cat));

    int status = AddProduct(db, pro);

    if (status == -1)
    {
        gtk_label_set_label(GTK_LABEL(AddProductLabel[5]), "Product Name Already Exists");
        gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[5]), "uncorrect");
        gtk_widget_remove_css_class(GTK_WIDGET(AddProductLabel[5]), "correct");
    }
    else if (status)
    {
        gtk_label_set_label(GTK_LABEL(AddProductLabel[5]), "Internal Server Error");
        gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[5]), "uncorrect");
        gtk_widget_remove_css_class(GTK_WIDGET(AddProductLabel[5]), "correct");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(AddProductLabel[5]), "Product Added Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[5]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(AddProductLabel[5]), "uncorrect");
    }
    
}

static void AddUs(GtkWidget *widget, gpointer user_data)
{
    User user;

    char *string_username = g_strdup(gtk_entry_buffer_get_text(AddUserEntryBuffer[0]));

    strcpy(user.UserName, string_username);

    char *string_pass = g_strdup(gtk_entry_buffer_get_text(AddUserEntryBuffer[1]));

    strcpy(user.UserPassword, string_pass);

    int status = AddUser(db, user);

    if (status == -1)
    {
        gtk_label_set_label(GTK_LABEL(AddUserLabel[2]), "User Name Already Exists");
        gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[2]), "uncorrect");
        gtk_widget_remove_css_class(GTK_WIDGET(AddUserLabel[2]), "correct");
    }
    else if (status)
    {
        gtk_label_set_label(GTK_LABEL(AddUserLabel[2]), "Internal Server Error");
        gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[2]), "uncorrect");
        gtk_widget_remove_css_class(GTK_WIDGET(AddUserLabel[2]), "correct");
    }
    else
    {
        gtk_label_set_label(GTK_LABEL(AddUserLabel[2]), "User Added Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[2]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(AddUserLabel[2]), "uncorrect");
    }
    
}

static void GoToViewUsers(GtkWidget *widget, gpointer user_data)
{
    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "box_view_users"))
    {
        if(gtk_widget_get_parent(GTK_WIDGET(ViewUsersFoot)) != NULL)
        {
            gtk_box_remove(GTK_BOX(ViewUsersBox), ViewUsersFoot);
        }

        if(ViewUsersSubBoxes)
        {
            for (int i = 0; i < n2; i++)
            {
                gtk_box_remove(GTK_BOX(ViewUsersSubBoxes[i]), ViewUsersLabel[i]);

                gtk_box_remove(GTK_BOX(ViewUsersBox), ViewUsersSubBoxes[i]);
            }

            free(ViewUsersSubBoxes);
            ViewUsersSubBoxes = NULL;

            free(ViewUsersLabel);
            ViewUsersLabel = NULL;

        }

        if(NoUserLabel)
        {
            gtk_box_remove(GTK_BOX(ViewUsersBox), *NoUserLabel);

            free(NoUserLabel);
            NoUserLabel = NULL;
        }

        DestroyUserList(&Users);
        GetUsers(&Users, db);

        if(Users.ElementsNumber)
        {
            ViewUsersSubBoxes = calloc(Users.ElementsNumber, sizeof(GtkWidget*));
            ViewUsersLabel = calloc(Users.ElementsNumber, sizeof(GtkWidget*));

            EUlist *P = Users.head;

            for (int i = 0; i < Users.ElementsNumber; i++)
            {
                ViewUsersSubBoxes[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_widget_set_size_request(ViewUsersSubBoxes[i], 600, 150);
                ViewUsersLabel[i] = gtk_label_new_with_mnemonic(P->User.UserName);
                gtk_widget_set_size_request(ViewUsersLabel[i], 400, 100);
                gtk_widget_add_css_class(GTK_WIDGET(ViewUsersLabel[i]), "alone-bold-label");
                gtk_widget_set_size_request(ViewUsersSubBoxes[i], 150, 100);
                
                gtk_box_append(GTK_BOX(ViewUsersSubBoxes[i]), ViewUsersLabel[i]);
                
                gtk_box_append(GTK_BOX(ViewUsersBox), ViewUsersSubBoxes[i]);

                P = P->next;
            }
        }
        else
        {
            NoUserLabel = malloc(sizeof(GtkWidget*));
            *NoUserLabel = gtk_label_new("No Users Added Yet\n");
            gtk_widget_add_css_class(GTK_WIDGET(*NoUserLabel), "alone-bold-label");
            gtk_box_append(GTK_BOX(ViewUsersBox), *NoUserLabel);
        }

        n2 = Users.ElementsNumber;

        ViewUsersFoot = gtk_label_new_with_mnemonic("\n");
        gtk_box_append(GTK_BOX(ViewUsersBox), ViewUsersFoot);

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_view_users");
    }
}

static void GoToViewProducts(GtkWidget *widget, gpointer user_data)
{
    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "box_view_products"))
    {
        if(gtk_widget_get_parent(GTK_WIDGET(ViewProductsFoot)) != NULL)
        {
            gtk_box_remove(GTK_BOX(ViewProductsBox), ViewProductsFoot);
        }

        if(ViewProductsSubBoxes)
        {
            for (int i = 0; i < n1; i++)
            {
                gtk_box_remove(GTK_BOX(ViewProductsSubBoxes[i]), ViewProductsLabel[i]);
                gtk_box_remove(GTK_BOX(ViewProductsSubBoxes[i]), DeleteProductsButton[i]);

                gtk_box_remove(GTK_BOX(ViewProductsBox), ViewProductsSubBoxes[i]);
            }

            free(ViewProductsSubBoxes);
            ViewProductsSubBoxes = NULL;

            free(ViewProductsLabel);
            ViewProductsLabel = NULL;

            free(DeleteProductsButton);
            DeleteProductsButton = NULL;
        }

        if(NoProductLabel)
        {
            gtk_box_remove(GTK_BOX(ViewProductsBox), *NoProductLabel);

            free(NoProductLabel);
            NoProductLabel = NULL;
        }

        DestroyProductList(&Products);
        GetProducts(&Products, db);

        if(Products.ElementsNumber)
        {
            char query[2000];
            ViewProductsSubBoxes = calloc(Products.ElementsNumber, sizeof(GtkWidget*));
            ViewProductsLabel = calloc(Products.ElementsNumber, sizeof(GtkWidget*));
            DeleteProductsButton = calloc(Products.ElementsNumber, sizeof(GtkWidget*));

            EPlist *P = Products.head;

            for (int i = 0; i < Products.ElementsNumber; i++)
            {
                ViewProductsSubBoxes[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_widget_set_size_request(ViewProductsSubBoxes[i], 600, 150);
                snprintf(query, sizeof(query), "%s \nDescription : %s \nPrice : %f \t Stock : %d", P->product.ProductName, P->product.Description, P->product.Price, P->product.Stock);

                ViewProductsLabel[i] = gtk_label_new_with_mnemonic(query);
                gtk_widget_set_size_request(ViewProductsLabel[i], 400, 100);
                gtk_widget_add_css_class(GTK_WIDGET(ViewProductsLabel[i]), "alone-bold-label");
                DeleteProductsButton[i] = gtk_button_new_with_label("Delete");
                gtk_widget_set_size_request(ViewProductsSubBoxes[i], 150, 100);
                g_signal_connect(DeleteProductsButton[i], "clicked", G_CALLBACK(DeletePro), GINT_TO_POINTER(i));
                
                gtk_box_append(GTK_BOX(ViewProductsSubBoxes[i]), ViewProductsLabel[i]);
                gtk_box_append(GTK_BOX(ViewProductsSubBoxes[i]), DeleteProductsButton[i]);
                
                gtk_box_append(GTK_BOX(ViewProductsBox), ViewProductsSubBoxes[i]);

                P = P->next;
            }
        }
        else
        {
            NoProductLabel = malloc(sizeof(GtkWidget*));
            *NoProductLabel = gtk_label_new("No Products Added Yet\n");
            gtk_widget_add_css_class(GTK_WIDGET(*NoProductLabel), "alone-bold-label");
            gtk_box_append(GTK_BOX(ViewProductsBox), *NoProductLabel);
        }

        n1 = Products.ElementsNumber;

        ViewProductsFoot = gtk_label_new_with_mnemonic("\n");
        gtk_box_append(GTK_BOX(ViewProductsBox), ViewProductsFoot);

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_view_products");
    }
}

static void GoToViewCategories(GtkWidget *widget, gpointer user_data)
{
    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "box_view_categories"))
    {
        if(gtk_widget_get_parent(GTK_WIDGET(ViewCategoriesFoot)) != NULL)
        {
            gtk_box_remove(GTK_BOX(ViewCategoriesBox), ViewCategoriesFoot);
        }

        if(ViewCategoriesSubBoxes)
        {
            for (int i = 0; i < n; i++)
            {
                gtk_box_remove(GTK_BOX(ViewCategoriesSubBoxes[i]), ViewCategoriesLabel[i]);
                gtk_box_remove(GTK_BOX(ViewCategoriesSubBoxes[i]), DeleteCategoriesButton[i]);

                gtk_box_remove(GTK_BOX(ViewCategoriesBox), ViewCategoriesSubBoxes[i]);
            }

            free(ViewCategoriesSubBoxes);
            ViewCategoriesSubBoxes = NULL;

            free(ViewCategoriesLabel);
            ViewCategoriesLabel = NULL;

            free(DeleteCategoriesButton);
            DeleteCategoriesButton = NULL;
        }

        if(NoCategoryLabel)
        {
            gtk_box_remove(GTK_BOX(ViewCategoriesBox), *NoCategoryLabel);

            free(NoCategoryLabel);
            NoCategoryLabel = NULL;
        }

        DestroyCategoryList(&Categories);
        GetCategories(&Categories, db);

        if(Categories.ElementsNumber)
        {
            ViewCategoriesSubBoxes = calloc(Categories.ElementsNumber, sizeof(GtkWidget*));
            ViewCategoriesLabel = calloc(Categories.ElementsNumber, sizeof(GtkWidget*));
            DeleteCategoriesButton = calloc(Categories.ElementsNumber, sizeof(GtkWidget*));
                
            EClist *P = Categories.head;

            for (int i = 0; i < Categories.ElementsNumber; i++)
            {
                ViewCategoriesSubBoxes[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_widget_set_size_request(ViewCategoriesSubBoxes[i], 600, 150);
                ViewCategoriesLabel[i] = gtk_label_new_with_mnemonic(P->category.CategoryName);
                gtk_widget_set_size_request(ViewCategoriesLabel[i], 400, 100);
                gtk_widget_add_css_class(GTK_WIDGET(ViewCategoriesLabel[i]), "alone-bold-label");
                DeleteCategoriesButton[i] = gtk_button_new_with_label("Delete");
                gtk_widget_set_size_request(ViewCategoriesSubBoxes[i], 150, 100);
                g_signal_connect(DeleteCategoriesButton[i], "clicked", G_CALLBACK(DeleteCat), GINT_TO_POINTER(i));
                
                gtk_box_append(GTK_BOX(ViewCategoriesSubBoxes[i]), ViewCategoriesLabel[i]);
                gtk_box_append(GTK_BOX(ViewCategoriesSubBoxes[i]), DeleteCategoriesButton[i]);
                
                gtk_box_append(GTK_BOX(ViewCategoriesBox), ViewCategoriesSubBoxes[i]);

                P = P->next;
            }
        }
        else
        {
            NoCategoryLabel = malloc(sizeof(GtkWidget*));
            *NoCategoryLabel = gtk_label_new("No Catategories Added Yet\n");
            gtk_widget_add_css_class(GTK_WIDGET(*NoCategoryLabel), "alone-bold-label");
            gtk_box_append(GTK_BOX(ViewCategoriesBox), *NoCategoryLabel);
        }

        n = Categories.ElementsNumber;

        ViewCategoriesFoot = gtk_label_new_with_mnemonic("\n");
        gtk_box_append(GTK_BOX(ViewCategoriesBox), ViewCategoriesFoot);

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_view_categories");
    }
    
}

static void DeleteCat(GtkWidget *widget, gpointer user_data)
{
    int rang = GPOINTER_TO_INT(user_data);
    printf("%d\n", rang);

    if(rang < Categories.ElementsNumber)
    {
        EClist *P = Categories.head;
        
        for(int i = 0; i < rang; i++)
        {
            P = P->next;
        }

        if(!DeleteCategoryByName(db, P->category.CategoryName))
        {
            printf("Success !!!\n");
            gtk_label_set_label(GTK_LABEL(BlankLabel), "");
            gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_blank");
            GoToViewCategories(NULL, NULL);
        }
    }
}

static void DeletePro(GtkWidget *widget, gpointer user_data)
{
    int rang = GPOINTER_TO_INT(user_data);
    printf("%d\n", rang);

    if(rang < Products.ElementsNumber)
    {
        EPlist *P = Products.head;
        
        for(int i = 0; i < rang; i++)
        {
            P = P->next;
        }

        if(!DeleteProductByName(db, P->product.ProductName))
        {
            printf("Success !!!\n");
            gtk_label_set_label(GTK_LABEL(BlankLabel), "");
            gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_blank");
            GoToViewProducts(NULL, NULL);
        }
    }
}

static void UpdateProStock(GtkWidget *widget, gpointer user_data)
{
    if(!UpdateStock(db, (char*)gtk_string_object_get_string(gtk_drop_down_get_selected_item(GTK_DROP_DOWN(UpdateProductDropdown))), atoi(g_strdup(gtk_entry_buffer_get_text(UpdateProductEntryBuffer[1])))))
    {
        gtk_label_set_label(GTK_LABEL(UpdateProductLabel[3]), "Product Stock Updated Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(UpdateProductLabel[3]), "uncorrect");
    }
}

static void UpdateProPrice(GtkWidget *widget, gpointer user_data)
{
    if(!UpdatePrice(db, (char*)gtk_string_object_get_string(gtk_drop_down_get_selected_item(GTK_DROP_DOWN(UpdateProductDropdown))), strtod(g_strdup(gtk_entry_buffer_get_text(UpdateProductEntryBuffer[0])), NULL)))
    {
        gtk_label_set_label(GTK_LABEL(UpdateProductLabel[3]), "Product Price Updated Successfully");
        gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[3]), "correct");
        gtk_widget_remove_css_class(GTK_WIDGET(UpdateProductLabel[3]), "uncorrect");
    }
}

static void GoToAddCategory(GtkWidget *widget, gpointer user_data)
{
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddCategoryEntryBuffer), "", 0);
    gtk_label_set_label(GTK_LABEL(AddCategoryLabel[2]), "");
    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "grid_add_category"))
    {
        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "grid_add_category");
    }
    
}

static void GoToAddUser(GtkWidget *widget, gpointer user_data)
{
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddUserEntryBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddUserEntryBuffer[1]), "", 0);
    gtk_label_set_label(GTK_LABEL(AddUserLabel[2]), "");
    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "grid_add_user"))
    {
        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "grid_add_user");
    }
    
}

static void GoToAddProduct(GtkWidget *widget, gpointer user_data)
{
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddProductEntryBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddProductEntryBuffer[1]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddProductEntryBuffer[2]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(AddProductEntryBuffer[3]), "", 0);
    gtk_label_set_label(GTK_LABEL(AddProductLabel[5]), "");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Dropdown), 0);

    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "grid_add_product"))
    {
        DestroyCategoryList(&Categories);
        GetCategories(&Categories, db);

        g_list_store_remove_all(store);

        EClist *P = Categories.head;
        GtkStringObject *label;

        if(!P)
        {
            gtk_label_set_label(GTK_LABEL(BlankLabel), "You should add Categories first !!!");
            gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_blank");
            return;
        }
        
        while(P)
        {
            label = gtk_string_object_new(P->category.CategoryName);
            g_list_store_append(store, label);
            P = P->next;
        }

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "grid_add_product");
    }
    
}

static void GoToUpdateProduct(GtkWidget *widget, gpointer user_data)
{
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UpdateProductEntryBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UpdateProductEntryBuffer[1]), "", 0);
    gtk_label_set_label(GTK_LABEL(AddProductLabel[5]), "");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(UpdateProductDropdown), 0);

    if (strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "grid_update_product"))
    {
        DestroyProductList(&Products);
        GetProducts(&Products, db);

        g_list_store_remove_all(UpdateStore);

        EPlist *P = Products.head;
        GtkStringObject *label;

        if(!P)
        {
            gtk_label_set_label(GTK_LABEL(BlankLabel), "You should add Products first !!!");
            gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "box_blank");
            return;
        }
        
        while(P)
        {
            label = gtk_string_object_new(P->product.ProductName);
            g_list_store_append(UpdateStore, label);
            P = P->next;
        }

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "grid_update_product");
    }
    
}

static void GoToLogin(GtkWidget *widget, gpointer user_data)
{
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[0]), "", 0);
    gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[1]), "", 0);

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

static void GoToLoggedAbout(GtkWidget *widget, gpointer user_data)
{
    if(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack[2])), "label_logged"))
    {
        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "label_logged");
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

        gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "label_logged");
        gtk_stack_set_visible_child_name(GTK_STACK(stack[0]), "box_logged");

        gtk_label_set_label(GTK_LABEL(UnloggedLabel[3]), "");
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[0]), "", 0);
        gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(UnloggedEntryBuffer[1]), "", 0);
    }
    
    DestroyUserList(&Users);
}

static void UnLog(GtkWidget *widget, gpointer user_data)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack[0]), "box_unlogged");
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

    LoggedLabel = gtk_label_new_with_mnemonic("This a simple C program that manages a store\n\nCreated By Zakaria Hammal");
    gtk_widget_add_css_class(GTK_WIDGET(LoggedLabel), "alone-bold-label");

    gtk_widget_set_hexpand(LoggedLabel, TRUE);
    gtk_widget_set_vexpand(LoggedLabel, TRUE);

    LoggedBtn[0] = gtk_button_new_with_label("View Categories");
    g_signal_connect (LoggedBtn[0], "clicked", G_CALLBACK(GoToViewCategories), NULL);
    LoggedBtn[1] = gtk_button_new_with_label("Add Categorie");
    g_signal_connect (LoggedBtn[1], "clicked", G_CALLBACK(GoToAddCategory), NULL);
    LoggedBtn[2] = gtk_button_new_with_label("View Products");
    g_signal_connect (LoggedBtn[2], "clicked", G_CALLBACK(GoToViewProducts), NULL);
    LoggedBtn[3] = gtk_button_new_with_label("Add Product");
    g_signal_connect (LoggedBtn[3], "clicked", G_CALLBACK(GoToAddProduct), NULL);
    LoggedBtn[4] = gtk_button_new_with_label("Update Product");
    g_signal_connect (LoggedBtn[4], "clicked", G_CALLBACK(GoToUpdateProduct), NULL);
    LoggedBtn[5] = gtk_button_new_with_label("View Users");
    g_signal_connect (LoggedBtn[5], "clicked", G_CALLBACK(GoToViewUsers), NULL);
    LoggedBtn[6] = gtk_button_new_with_label("Add User");
    g_signal_connect (LoggedBtn[6], "clicked", G_CALLBACK(GoToAddUser), NULL);
    LoggedBtn[7] = gtk_button_new_with_label("Diconnect");
    g_signal_connect (LoggedBtn[7], "clicked", G_CALLBACK(UnLog), NULL);
    LoggedBtn[8] = gtk_button_new_with_label("About");
    g_signal_connect (LoggedBtn[8], "clicked", G_CALLBACK(GoToLoggedAbout), NULL);

    gtk_box_append(GTK_BOX(sidebar[1]), UsernameLabel);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[8]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[0]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[1]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[2]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[3]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[4]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[5]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[6]);
    gtk_box_append(GTK_BOX(sidebar[1]), LoggedBtn[7]);

    gtk_stack_add_titled(GTK_STACK(stack[2]), LoggedLabel, "label_logged", "loggedAbout");

    BlankBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    BlankLabel = gtk_label_new("");
    gtk_widget_add_css_class(GTK_WIDGET(BlankLabel), "uncorrect");
    gtk_widget_add_css_class(GTK_WIDGET(BlankLabel), "bold-label");
    gtk_box_append(GTK_BOX(BlankBox), BlankLabel);

    // Categories :
    
    ViewCategoriesBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    ViewCategoriesHead = gtk_label_new_with_mnemonic("\nCategories :\n");
    gtk_widget_add_css_class(GTK_WIDGET(ViewCategoriesHead), "title-label");
    ViewCategoriesFoot = gtk_label_new_with_mnemonic("\n");

    gtk_box_append(GTK_BOX(ViewCategoriesBox), GTK_WIDGET(ViewCategoriesHead));

    scrolled_window[0] = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[0]), 
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC
    );

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window[0]), ViewCategoriesBox);

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

    gtk_stack_add_titled(GTK_STACK(stack[2]), scrolled_window[0], "box_view_categories", "ViewCategories");
    gtk_stack_add_titled(GTK_STACK(stack[2]), AddCategoryGrid, "grid_add_category", "AddCategory");
    gtk_stack_add_titled(GTK_STACK(stack[2]), BlankBox, "box_blank", "BlankBox");

    // Products :

    ViewProductsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    ViewProductsHead = gtk_label_new_with_mnemonic("\nProducts :\n");
    gtk_widget_add_css_class(GTK_WIDGET(ViewProductsHead), "title-label");

    ViewProductsFoot = gtk_label_new_with_mnemonic("\n");

    gtk_box_append(GTK_BOX(ViewProductsBox), GTK_WIDGET(ViewProductsHead));

    scrolled_window[1] = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[1]), 
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC
    );

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window[1]), ViewProductsBox);

    AddProductGrid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(AddProductGrid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(AddProductGrid), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(AddProductGrid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(AddProductGrid), FALSE);

    AddProductLabel[0] = gtk_label_new("\nProduct Name :");
    gtk_widget_set_size_request(AddProductLabel[0], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[0]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[0]), 0.0);
    AddProductLabel[1] = gtk_label_new("\nDescription :");
    gtk_widget_set_size_request(AddProductLabel[1], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[1]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[1]), 0.0);
    AddProductLabel[2] = gtk_label_new("\nStock :");
    gtk_widget_set_size_request(AddProductLabel[2], 200, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[2]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[2]), 0.0);
    AddProductLabel[3] = gtk_label_new("\nPrice :");
    gtk_widget_set_size_request(AddProductLabel[3], 200, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[3]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[3]), 0.0);
    AddProductLabel[4] = gtk_label_new("\nCategory :");
    gtk_widget_set_size_request(AddProductLabel[4], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[4]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[4]), 0.0);
    AddProductLabel[5] = gtk_label_new("");
    gtk_widget_add_css_class(GTK_WIDGET(AddProductLabel[5]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddProductLabel[5]), 0.0);

    store = g_list_store_new(GTK_TYPE_STRING_OBJECT);
    Dropdown = gtk_drop_down_new(G_LIST_MODEL(store), NULL);

    AddProductEntryBuffer[0] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddProductEntryBuffer[0]), 40);

    AddProductEntry[0] = gtk_entry_new_with_buffer(AddProductEntryBuffer[0]);

    AddProductEntryBuffer[1] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddProductEntryBuffer[1]), 1000);

    AddProductEntry[1] = gtk_entry_new_with_buffer(AddProductEntryBuffer[1]);
    gtk_widget_set_size_request(AddProductEntry[1], 500, 20);

    AddProductEntryBuffer[2] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddProductEntryBuffer[2]), 10);

    AddProductEntry[2] = gtk_entry_new_with_buffer(AddProductEntryBuffer[2]);
    gtk_widget_set_size_request(AddProductEntry[2], 200, 20);

    AddProductEntryBuffer[3] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddProductEntryBuffer[3]), 10);

    AddProductEntry[3] = gtk_entry_new_with_buffer(AddProductEntryBuffer[3]);
    gtk_widget_set_size_request(AddProductEntry[3], 200, 20);

    AddProductButton = gtk_button_new_with_label("Add Product");
    g_signal_connect(AddProductButton, "clicked", G_CALLBACK(AddPro), NULL);

    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[0]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductEntry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[1]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductEntry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[4]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(Dropdown), 1, 6, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[2]), 1, 7, 4, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductEntry[2]), 6, 7, 4, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[3]), 1, 8, 4, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductEntry[3]), 6, 8, 4, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductLabel[5]), 1, 9, 10, 1);
    gtk_grid_attach(GTK_GRID(AddProductGrid), GTK_WIDGET(AddProductButton), 5, 10, 10, 1);

    UpdateProductGrid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(UpdateProductGrid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(UpdateProductGrid), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(UpdateProductGrid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(UpdateProductGrid), FALSE);

    UpdateProductLabel[0] = gtk_label_new("\nProduct Name :");
    gtk_widget_set_size_request(UpdateProductLabel[0], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[0]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UpdateProductLabel[0]), 0.0);

    UpdateProductLabel[1] = gtk_label_new("\nNew Price :");
    gtk_widget_set_size_request(UpdateProductLabel[1], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[1]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UpdateProductLabel[1]), 0.0);

    UpdateProductLabel[2] = gtk_label_new("\nNew Stock :");
    gtk_widget_set_size_request(UpdateProductLabel[2], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[2]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UpdateProductLabel[2]), 0.0);

    UpdateProductLabel[3] = gtk_label_new("");
    gtk_widget_add_css_class(GTK_WIDGET(UpdateProductLabel[3]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(UpdateProductLabel[3]), 0.0);

    UpdateStore = g_list_store_new(GTK_TYPE_STRING_OBJECT);
    UpdateProductDropdown = gtk_drop_down_new(G_LIST_MODEL(UpdateStore), NULL);

    UpdateProductEntryBuffer[0] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(UpdateProductEntryBuffer[0]), 10);

    UpdateProductEntry[0] = gtk_entry_new_with_buffer(UpdateProductEntryBuffer[0]);
    gtk_widget_set_size_request(UpdateProductEntry[0], 200, 20);

    UpdateProductEntryBuffer[1] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(UpdateProductEntryBuffer[1]), 10);

    UpdateProductEntry[1] = gtk_entry_new_with_buffer(UpdateProductEntryBuffer[1]);
    gtk_widget_set_size_request(UpdateProductEntry[1], 200, 20);

    UpdateProductButton[0] = gtk_button_new_with_label("Update Stock");
    g_signal_connect(UpdateProductButton[0], "clicked", G_CALLBACK(UpdateProStock), NULL);
    UpdateProductButton[1] = gtk_button_new_with_label("Update Price");
    g_signal_connect(UpdateProductButton[1], "clicked", G_CALLBACK(UpdateProPrice), NULL);

    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductLabel[0]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductDropdown), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductLabel[1]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductEntry[0]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductLabel[2]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductEntry[1]), 1, 6, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductLabel[3]), 1, 7, 10, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductButton[0]), 1, 8, 5, 1);
    gtk_grid_attach(GTK_GRID(UpdateProductGrid), GTK_WIDGET(UpdateProductButton[1]), 7, 8, 5, 1);

    gtk_stack_add_titled(GTK_STACK(stack[2]), UpdateProductGrid, "grid_update_product", "UpdateProduct");
    gtk_stack_add_titled(GTK_STACK(stack[2]), AddProductGrid, "grid_add_product", "AddProduct");
    gtk_stack_add_titled(GTK_STACK(stack[2]), scrolled_window[1], "box_view_products", "ViewProducts");

    // Users :

    ViewUsersBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    ViewUsersHead = gtk_label_new_with_mnemonic("\nUsers :\n");
    gtk_widget_add_css_class(GTK_WIDGET(ViewUsersHead), "title-label");

    ViewUsersFoot = gtk_label_new_with_mnemonic("\n");

    gtk_box_append(GTK_BOX(ViewUsersBox), GTK_WIDGET(ViewUsersHead));

    scrolled_window[2] = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[2]), 
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC
    );

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window[2]), ViewUsersBox);

    AddUserGrid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(AddUserGrid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(AddUserGrid), 10);

    gtk_grid_set_row_homogeneous(GTK_GRID(AddUserGrid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(AddUserGrid), FALSE);

    AddUserLabel[0] = gtk_label_new("\nUser Name :");
    gtk_widget_set_size_request(AddUserLabel[0], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[0]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddUserLabel[0]), 0.0);
    AddUserLabel[1] = gtk_label_new("\nPassword :");
    gtk_widget_set_size_request(AddUserLabel[1], 500, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[1]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddUserLabel[1]), 0.0);
    AddUserLabel[2] = gtk_label_new("");
    gtk_widget_set_size_request(AddUserLabel[2], 200, 20);
    gtk_widget_add_css_class(GTK_WIDGET(AddUserLabel[2]), "bold-label");
    gtk_label_set_xalign(GTK_LABEL(AddUserLabel[2]), 0.0);

    AddUserEntryBuffer[0] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddUserEntryBuffer[0]), 40);

    AddUserEntry[0] = gtk_entry_new_with_buffer(AddUserEntryBuffer[0]);

    AddUserEntryBuffer[1] = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(AddUserEntryBuffer[1]), 40);

    AddUserEntry[1] = gtk_entry_new_with_buffer(AddUserEntryBuffer[1]);
    gtk_widget_set_size_request(AddUserEntry[1], 500, 20);

    AddUserEntryBuffer[2] = gtk_entry_buffer_new(NULL, -1);

    AddUserButton = gtk_button_new_with_label("Add Product");
    g_signal_connect(AddUserButton, "clicked", G_CALLBACK(AddUs), NULL);

    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserLabel[0]), 1, 1, 10, 1);
    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserEntry[0]), 1, 2, 10, 1);
    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserLabel[1]), 1, 3, 10, 1);
    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserEntry[1]), 1, 4, 10, 1);
    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserLabel[2]), 1, 5, 10, 1);
    gtk_grid_attach(GTK_GRID(AddUserGrid), GTK_WIDGET(AddUserButton), 5, 6, 4, 1);

    gtk_stack_add_titled(GTK_STACK(stack[2]), scrolled_window[2], "box_view_users", "ViewUsers");
    gtk_stack_add_titled(GTK_STACK(stack[2]), AddUserGrid, "grid_add_user", "AddUser");

    gtk_stack_set_visible_child_name(GTK_STACK(stack[2]), "label_logged");

    gtk_widget_set_hexpand(stack[2], TRUE);
    gtk_widget_set_vexpand(stack[2], TRUE);

    gtk_box_append(GTK_BOX(boxLogged), sidebar[1]);
    gtk_box_append(GTK_BOX(boxLogged), stack[2]);

    gtk_stack_add_titled(GTK_STACK(stack[0]), boxLogged, "box_logged", "Logged");
    gtk_stack_add_titled(GTK_STACK(stack[0]), boxUnlogged, "box_unlogged", "Unlogged");

    gtk_stack_set_visible_child_name(GTK_STACK(stack[0]), "box_unlogged");

    gtk_window_set_child(GTK_WINDOW(window), stack[0]);

    gtk_window_present(GTK_WINDOW(window));
}

static void on_app_shutdown(GApplication *app, gpointer user_data) 
{
    DestroyCategoryList(&Categories);
    DestroyProductList(&Products);
    DestroyUserList(&Users);

    if(ViewUsersSubBoxes)
    {
        free(ViewUsersSubBoxes);
        ViewUsersSubBoxes = NULL;

        free(ViewUsersLabel);
        ViewUsersLabel = NULL;

    }

    if(NoUserLabel)
    {
        free(NoUserLabel);
        NoUserLabel = NULL;
    }

    if(ViewProductsSubBoxes)
    {
        free(ViewProductsSubBoxes);
        ViewProductsSubBoxes = NULL;

        free(ViewProductsLabel);
        ViewProductsLabel = NULL;

        free(DeleteProductsButton);
        DeleteProductsButton = NULL;
    }

    if(NoProductLabel)
    {
        free(NoProductLabel);
        NoProductLabel = NULL;
    }

    if(ViewCategoriesSubBoxes)
    {
        free(ViewCategoriesSubBoxes);
        ViewCategoriesSubBoxes = NULL;

        free(ViewCategoriesLabel);
        ViewCategoriesLabel = NULL;

        free(DeleteCategoriesButton);
        DeleteCategoriesButton = NULL;
    }

    if(NoCategoryLabel)
    {
        free(NoCategoryLabel);
        NoCategoryLabel = NULL;
    }
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

    NoCategoryLabel = NULL;
    DeleteCategoriesButton = NULL;
    ViewCategoriesSubBoxes = NULL;

    NoProductLabel = NULL;
    DeleteProductsButton = NULL;
    ViewProductsSubBoxes = NULL;

    NoUserLabel = NULL;
    ViewUsersSubBoxes = NULL;

    GtkApplication *app;
    int status;

    app = gtk_application_new ("stackof.holger.entry", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_app_shutdown), NULL);

    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
