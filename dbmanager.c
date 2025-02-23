#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "dbmanager.h"

int GetCategories(Clist *Categories, sqlite3 *db)
{
    (*Categories).ElementsNumber = 0;
    (*Categories).head = NULL;

    EClist *P = NULL;
    EClist *Q = NULL;

    sqlite3_stmt *stmt;
    const char *query = "SELECT CategoryName FROM Categories;";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            (*Categories).head = malloc(sizeof(EClist));
            strcpy((*Categories).head->category.CategoryName, (const char*)sqlite3_column_text(stmt, 0));
            (*Categories).head->next = NULL;
            P = (*Categories).head;
            (*Categories).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EClist));
            strcpy(Q->category.CategoryName, (const char*)sqlite3_column_text(stmt, 0));
            P->next = Q;
            P = Q;
            (*Categories).ElementsNumber += 1;
        }

        if (Q)
        {
            Q->next = NULL;
        }
        
    }

    sqlite3_finalize(stmt);

    return 0;
}

EClist* GetCategoryByName(Clist categories, char categoryName[])
{
    EClist *P = categories.head;

    while (P)
    {
        if (!strcmp(P->category.CategoryName, categoryName))
        {
            return P;
        }
        
        P = P->next;
    }
    
    return NULL;
}

int GetProducts(Plist *Products, sqlite3 *db)
{
    (*Products).ElementsNumber = 0;
    (*Products).head = NULL;

    EPlist *P = NULL;
    EPlist *Q = NULL;

    sqlite3_stmt *stmt;
    const char *query = "SELECT ProductName, Description, Price, Stock, CategoryName FROM Products;";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            (*Products).head = malloc(sizeof(EPlist));
            strcpy((*Products).head->product.ProductName, (const char*)sqlite3_column_text(stmt, 0));
            strcpy((*Products).head->product.Description, (const char*)sqlite3_column_text(stmt, 1));
            (*Products).head->product.Price = sqlite3_column_double(stmt, 2);
            (*Products).head->product.Stock = sqlite3_column_int(stmt, 3);
            strcpy((*Products).head->product.CategoryName, sqlite3_column_text(stmt, 4));

            (*Products).head->next = NULL;
            P = (*Products).head;
            (*Products).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EPlist));
            strcpy(Q->product.ProductName, (const char*)sqlite3_column_text(stmt, 0));
            strcpy(Q->product.Description, (const char*)sqlite3_column_text(stmt, 1));
            Q->product.Price = sqlite3_column_double(stmt, 2);
            Q->product.Stock = sqlite3_column_int(stmt, 3);
            strcpy(Q->product.CategoryName, sqlite3_column_text(stmt, 4));
            
            P->next = Q;
            P = Q;
            (*Products).ElementsNumber += 1;
        }

        if (Q)
        {
            Q->next = NULL;
        }
        
    }

    sqlite3_finalize(stmt);

    return 0;
}

EPlist* GetProductByName(Plist products, char productName[])
{
    EPlist *P = products.head;

    while (P)
    {
        if (!strcmp(P->product.ProductName, productName))
        {
            return P;
        }
        
        P = P->next;
    }
    
    return NULL;
}

int GetUsers(Ulist *Users, sqlite3 *db)
{
    (*Users).ElementsNumber = 0;
    (*Users).head = NULL;

    EUlist *P = NULL;
    EUlist *Q = NULL;

    sqlite3_stmt *stmt;
    const char *query = "SELECT UserName, UserPassword FROM Users;";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            (*Users).head = malloc(sizeof(EUlist));
            strcpy((*Users).head->User.UserName, (const char*)sqlite3_column_text(stmt, 0));
            strcpy((*Users).head->User.UserPassword, (const char*)sqlite3_column_text(stmt, 1));
            (*Users).head->next = NULL;
            P = (*Users).head;
            (*Users).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EUlist));
            strcpy(Q->User.UserName, (const char*)sqlite3_column_text(stmt, 0));
            strcpy(Q->User.UserPassword, (const char*)sqlite3_column_text(stmt, 1));
            P->next = Q;
            P = Q;
            (*Users).ElementsNumber += 1;
        }

        if (Q)
        {
            Q->next = NULL;
        }
    }

    sqlite3_finalize(stmt);

    return 0;
}

EUlist* GetUserByName(Ulist users, char userName[])
{
    EUlist *P = users.head;

    while (P)
    {
        if (!strcmp(P->User.UserName, userName))
        {
            return P;
        }
        
        P = P->next;
    }
    
    return NULL;
}

int LogInUser(Ulist users, char username[], char password[])
{
    EUlist *P = users.head;
    while (P && (strcmp(P->User.UserName, username) != 0 || strcmp(P->User.UserPassword, password) != 0))
    {
        P = P->next;
    }
    
    return (P == NULL);
}

int AddCategorie(sqlite3 *db, Category category)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM Categories WHERE CategoryName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, category.CategoryName, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    sqlite3_finalize(stmt);
    
    sqlite3_prepare_v2(db,
        "INSERT INTO Categories (CategoryName) Values (?);",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, category.CategoryName, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return 0;
}

int AddProduct(sqlite3 *db, Product product)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM Products WHERE ProductName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, product.ProductName, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        printf("%d\n", count);
        if (count > 0) {
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM Categories WHERE CategoryName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, product.CategoryName, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count == 0) {
            sqlite3_finalize(stmt);
            return 2;
        }
    }

    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db,
        "INSERT INTO Products (ProductName, Description, Price, Stock, CategoryName) Values (?, ?, ?, ?, ?);",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, product.ProductName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, product.Description, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, product.Price);
    sqlite3_bind_int(stmt, 4, product.Stock);
    sqlite3_bind_text(stmt, 5, product.CategoryName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return 0;
}

int AddUser(sqlite3 *db, User user)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM Users WHERE UserName = ?;",
        -1, &stmt, NULL
    );

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db,
        "INSERT INTO Users (UserName, UserPassword) VALUES (?, ?);",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, user.UserName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.UserPassword, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return 0;
}

int UpdatePrice(sqlite3 *db, char productName[], double newPrice)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "UPDATE Products SET Price = ? WHERE ProductName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_double(stmt, 1, newPrice);
    sqlite3_bind_text(stmt, 2, productName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        return - 1;
    }

    return 0;
}

int UpdateStock(sqlite3 *db, char productName[], int newStock)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "UPDATE Products SET Stock = ? WHERE ProductName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_double(stmt, 1, newStock);
    sqlite3_bind_text(stmt, 2, productName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        return - 1;
    }

    return 0;
}

int DeleteCategoryByName(sqlite3 *db, char categoryName[])
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "DELETE FROM Categories WHERE CategoryName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, categoryName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        return - 1;
    }

    return 0;
}

int DeleteProductByName(sqlite3 *db, char productName[])
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "DELETE FROM Products WHERE ProductName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, productName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        return - 1;
    }

    return 0;
}

int DeleteUserByName(sqlite3 *db, char userName[])
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "DELETE FROM Users WHERE UserName = ?;",
        -1, &stmt, NULL
    );

    sqlite3_bind_text(stmt, 1, userName, -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int changes = sqlite3_changes(db);
    if (changes == 0)
    {
        return - 1;
    }

    return 0;
}
