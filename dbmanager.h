#ifndef DATABASE
#define DATABASE

typedef struct Category Category;
struct Category
{
    int CategoryID;
    char CategoryName[41];
};

typedef struct Product Product;
struct Product
{
    int ProductID;
    char ProductName[41];
    char Description[1001];
    double Price;
    int Stock;
    int CategoryID;
    char CategoryName[41];
};

typedef struct User User;
struct User
{
    int UserID;
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
EClist* GetCategoryByID(Clist categories, int categoryID);

int GetProducts(Plist *Products, sqlite3 *db, Clist categories);
EPlist* GetProductByID(Plist products, int productID);

int GetUsers(Ulist *Users, sqlite3 *db);
EUlist* GetUserByID(Ulist users, int userID);

int LogInUser(Ulist users, char username[], char password[]);

int AddCategorie(sqlite3 *db, Category category);
int AddProduct(sqlite3 *db, Product product);
int AddUser(sqlite3 *db, User user);

int UpdatePrice(sqlite3 *db, int productID, double newPrice);
int UpdateStock(sqlite3 *db, int productID, int newStock);

#endif