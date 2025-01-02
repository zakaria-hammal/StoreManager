# GTK4 C Program with SQLite3

A C program using GTK4 for the graphical user interface (GUI) and SQLite3 for database management. This project demonstrates how to combine a modern GUI with a lightweight database.

---

## Prerequisites

Before you can compile and run this project, ensure the following tools and libraries are installed:

1. **GCC**: The GNU Compiler Collection.
2. **GTK4**: The GTK4 development libraries.
3. **SQLite3**: The SQLite3 development library.
4. **pkg-config**: Used to retrieve the necessary compiler and linker flags.

### Creating the Database
This app uses Sqlite database, to create the database download sqlite and type in Command Prompt : 
```bash
sqlite3 store.db < store.sql
```

### Compiling the Program
To compile the program, use the following command:
```bash
gcc $(pkg-config --cflags gtk4) -o prog main.c dbmanager.c $(pkg-config --libs gtk4) -lsqlite3
```

After compiling, run the program with:
```bash
./prog

