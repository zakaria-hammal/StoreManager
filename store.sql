DROP TABLE IF EXISTS "Categories";
DROP TABLE IF EXISTS "Products";
DROP TABLE IF EXISTS "Users";

CREATE TABLE "Categories" (
	"CategoryID" INTEGER PRIMARY KEY,
	"CategoryName" nvarchar (40) NOT NULL
);

CREATE TABLE "Products" (
	"ProductID" INTEGER PRIMARY KEY ,
	"ProductName" nvarchar (40) NOT NULL ,
    "Description" "ntext" NULL ,
    "Price" REAL NULL CONSTRAINT "DF_Products_Cost" DEFAULT (0),
    "Stock" INTEGER NULL CONSTRAINT "DF_Products_Stock" DEFAULT (0),
    "CategoryID" INTEGER,
    FOREIGN KEY ("CategoryID") REFERENCES Categories("CategoryID") ON DELETE CASCADE,
    
    CONSTRAINT "CK_Products_Cost" CHECK (Price >= 0),
    CONSTRAINT "CK_Products_Stock" CHECK (Stock >= 0)
);

CREATE TABLE "Users" (
    "UserID" INTEGER PRIMARY KEY ,
    "UserName" nvarchar (40) NOT NULL UNIQUE,
    "UserPassword" nvarchar (40) NOT NULL
);

INSERT INTO Users (UserName, UserPassword) VALUES ('Zakaria', 'BestOne2006');