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
    const char *query = "SELECT CategoryID, CategoryName FROM Categories;";

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
            (*Categories).head->category.CategoryID = sqlite3_column_int(stmt, 0);
            strcpy((*Categories).head->category.CategoryName, (const char*)sqlite3_column_text(stmt, 1));
            (*Categories).head->next = NULL;
            P = (*Categories).head;
            (*Categories).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EClist));
            Q->category.CategoryID = sqlite3_column_int(stmt, 0);
            strcpy(Q->category.CategoryName, (const char*)sqlite3_column_text(stmt, 1));
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

EClist* GetCategoryByID(Clist categories, int categoryID)
{
    EClist *P = categories.head;

    while (P)
    {
        if (P->category.CategoryID == categoryID)
        {
            return P;
        }
        
        P = P->next;
    }
    
    return NULL;
}

int GetProducts(Plist *Products, sqlite3 *db, Clist categories)
{
    (*Products).ElementsNumber = 0;
    (*Products).head = NULL;

    EPlist *P = NULL;
    EPlist *Q = NULL;
    EClist *C = NULL;

    sqlite3_stmt *stmt;
    const char *query = "SELECT ProductID, ProductName, Description, Price, Stock, CategoryID FROM Products;";

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
            (*Products).head->product.ProductID = sqlite3_column_int(stmt, 0);
            strcpy((*Products).head->product.ProductName, (const char*)sqlite3_column_text(stmt, 1));
            strcpy((*Products).head->product.Description, (const char*)sqlite3_column_text(stmt, 2));
            (*Products).head->product.Price = sqlite3_column_double(stmt, 3);
            (*Products).head->product.Stock = sqlite3_column_int(stmt, 4);
            (*Products).head->product.CategoryID = sqlite3_column_int(stmt, 5);
            C = GetCategoryByID(categories, (*Products).head->product.CategoryID);
            
            if (!C)
            {
                fprintf(stderr, "Internal Database Error\n");
                return 1;
            }

            strcpy((*Products).head->product.CategoryName, C->category.CategoryName);
            
            (*Products).head->next = NULL;
            P = (*Products).head;
            (*Products).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EPlist));
            Q->product.ProductID = sqlite3_column_int(stmt, 0);
            strcpy(Q->product.ProductName, (const char*)sqlite3_column_text(stmt, 1));
            strcpy(Q->product.Description, (const char*)sqlite3_column_text(stmt, 2));
            Q->product.Price = sqlite3_column_double(stmt, 3);
            Q->product.Stock = sqlite3_column_int(stmt, 4);
            Q->product.CategoryID = sqlite3_column_int(stmt, 5);
            C = GetCategoryByID(categories, Q->product.CategoryID);
            
            if (!C)
            {
                fprintf(stderr, "Internal Database Error\n");
                return 1;
            }

            strcpy(Q->product.CategoryName, C->category.CategoryName);
            
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

EPlist* GetProductByID(Plist products, int productID)
{
    EPlist *P = products.head;

    while (P)
    {
        if (P->product.ProductID == productID)
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
    const char *query = "SELECT UserID, UserName, UserPassword FROM Users;";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            (*Users).head = malloc(sizeof(EClist));
            (*Users).head->User.UserID = sqlite3_column_int(stmt, 0);
            strcpy((*Users).head->User.UserName, (const char*)sqlite3_column_text(stmt, 1));
            strcpy((*Users).head->User.UserPassword, (const char*)sqlite3_column_text(stmt, 2));
            (*Users).head->next = NULL;
            P = (*Users).head;
            (*Users).ElementsNumber += 1;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Q = malloc(sizeof(EClist));
            Q->User.UserID = sqlite3_column_int(stmt, 0);
            strcpy(Q->User.UserName, (const char*)sqlite3_column_text(stmt, 1));
            strcpy(Q->User.UserPassword, (const char*)sqlite3_column_text(stmt, 2));
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

EUlist* GetUserByID(Ulist users, int userID)
{
    EUlist *P = users.head;

    while (P)
    {
        if (P->User.UserID == userID)
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
    while (P && strcmp(P->User.UserName, username) != 0 && strcmp(P->User.UserPassword, password) != 0)
    {
        P = P->next;
    }
    
    return (P == NULL);
}

int AddCategorie(sqlite3 *db, Category category)
{
    char query[2000];
    snprintf(query, sizeof(query), "INSERT INTO Categories (CategoryName) Values ('%s');", category.CategoryName);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int AddProduct(sqlite3 *db, Product product)
{
    char query[2000];
    snprintf(query, sizeof(query), "INSERT INTO Products (ProductName, Description, Price, Stock, CategoryID) Values ('%s', '%s', '%2.f', '%d', '%d');", product.ProductName, product.Description, product.Price, product.Stock, product.CategoryID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int AddUser(sqlite3 *db, User user)
{
    char query[2000];

    snprintf(query, sizeof(query), "INSERT INTO Users (UserName, UserPassword) VALUES ('%s', '%s');", user.UserName, user.UserPassword);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int UpdatePrice(sqlite3 *db, int productID, double newPrice)
{
    char query[2000];
    snprintf(query, sizeof(query), "UPDATE Products SET Price = %2.f WHERE ProductID = %d;", newPrice, productID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int UpdateStock(sqlite3 *db, int productID, int newStock)
{
    char query[2000];
    snprintf(query, sizeof(query), "UPDATE Products SET Stock = %d WHERE ProductID = %d;", newStock, productID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int DeleteCategoryByID(sqlite3 *db, int categoryID)
{
    char query[2000];
    snprintf(query, sizeof(query), "DELETE FROM Categories WHERE CategoryID = %d;", categoryID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int DeleteProductByID(sqlite3 *db, int productID)
{
    char query[2000];
    snprintf(query, sizeof(query), "DELETE FROM Products WHERE ProductID = %d;", productID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int DeleteUserByID(sqlite3 *db, int userID)
{
    char query[2000];
    snprintf(query, sizeof(query), "DELETE FROM Users WHERE UserID = %d;", userID);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int DeleteUserByUsername(sqlite3 *db, char username[])
{
    char query[2000];
    snprintf(query, sizeof(query), "DELETE FROM Users WHERE UserName = '%s';", username);

    char *errMsg = 0;
    
    if (sqlite3_exec(db, query, 0, 0, &errMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}