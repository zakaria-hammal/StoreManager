#ifndef DATABASE
#define DATABASE

typedef struct Category Category;
struct Category
{
    char CategoryName[41];
};

typedef struct Product Product;
struct Product
{
    char ProductName[41];
    char Description[1001];
    double Price;
    int Stock;
    char CategoryName[41];
};

typedef struct User User;
struct User
{
    char UserName[41];
    char UserPassword[41];
};

typedef struct EClist EClist;
struct EClist
{
    Category category;
    EClist *next;
};

typedef struct Clist Clist;
struct Clist
{
    EClist *head;
    int ElementsNumber;
};

typedef struct EPlist EPlist;
struct EPlist
{
    Product product;
    EPlist *next;
};

typedef struct Plist Plist;
struct Plist
{
    EPlist *head;
    int ElementsNumber;
};

typedef struct EUlist EUlist;
struct EUlist
{
    User User;
    EUlist *next;
};

typedef struct Ulist Ulist;
struct Ulist
{
    EUlist *head;
    int ElementsNumber;
};

int GetCategories(Clist *Categories, sqlite3 *db);
EClist* GetCategoryByName(Clist categories, char categoryName[]);

int GetProducts(Plist *Products, sqlite3 *db);
EPlist* GetProductByName(Plist products, char productName[]);

int GetUsers(Ulist *Users, sqlite3 *db);
EUlist* GetUserByName(Ulist users, char userName[]);

int LogInUser(Ulist users, char username[], char password[]);

int AddCategorie(sqlite3 *db, Category category);
int AddProduct(sqlite3 *db, Product product);
int AddUser(sqlite3 *db, User user);

int UpdatePrice(sqlite3 *db, char productName[], double newPrice);
int UpdateStock(sqlite3 *db, char productName[], int newStock);

int DeleteCategoryByName(sqlite3 *db, char categoryName[]);
int DeleteProductByName(sqlite3 *db, char productName[]);
int DeleteUserByName(sqlite3 *db, char userName[]);

#endif