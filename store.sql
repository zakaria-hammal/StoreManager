DROP TABLE IF EXISTS "Categories";
DROP TABLE IF EXISTS "Products";
DROP TABLE IF EXISTS "Users";

CREATE TABLE "Categories" (
	"CategoryName" nvarchar (40) NOT NULL PRIMARY KEY
);

CREATE TABLE "Products" (
	"ProductName" nvarchar (40) NOT NULL PRIMARY KEY,
    "Description" TEXT NULL,
    "Price" REAL NULL CONSTRAINT "DF_Products_Cost" DEFAULT (0),
    "Stock" INTEGER NULL CONSTRAINT "DF_Products_Stock" DEFAULT (0),
    "CategoryName" nvarchar (40),
    FOREIGN KEY ("CategoryName") REFERENCES Categories("CategoryName") ON DELETE CASCADE,
    
    CONSTRAINT "CK_Products_Cost" CHECK (Price >= 0),
    CONSTRAINT "CK_Products_Stock" CHECK (Stock >= 0)
);

CREATE TABLE "Users" (
    "UserName" nvarchar (40) NOT NULL PRIMARY KEY,
    "UserPassword" nvarchar (40) NOT NULL
);

INSERT INTO Users (UserName, UserPassword) VALUES ('Zakaria', 'BestOne2006');
