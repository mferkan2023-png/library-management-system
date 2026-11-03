# Library Management System

ICT Lab semester project, Fall 2026.
A console program in C++ that uses an SQLite database.

Student: Muhammed Fatih Erkan (01-136262-054)

## Features
- Add, view, search, update and delete books and members
- Issue and return books (loans)
- Search books by ID, title, author or category
- Generate 3500 test records (2000 books, 500 members, 1000 loans)
- Speed test that compares four search queries

## Files
| File | What it does |
| --- | --- |
| main.cpp | Console menu |
| Database.h / Database.cpp | All SQL queries |
| Models.h | Book, Member and Loan classes |
| DataGenerator.h / .cpp | Makes the test data |
| sqlite3.c / sqlite3.h | SQLite library (version 3.38.2) |
| library.db | SQLite database with 3500 records |
| diagrams/ | ERD and class diagram |
| Report.docx | Project report |

## How to run in Visual Studio
1. Create a new **Empty Project** (C++).
2. Copy all .cpp, .h and .c files into the project folder.
3. In Solution Explorer: right click **Source Files** > Add > Existing Item, and add main.cpp, Database.cpp, DataGenerator.cpp and sqlite3.c. Add the .h files to **Header Files**.
4. Build and run (Ctrl + F5).
5. Choose **6** to see the number of records, and **5** for the speed test.

`library.db` is already included with 3500 records (2000 books, 500 members, 1000 loans). Keep it in the same folder as the .cpp files. If you delete it, the program makes a new empty database, and you can fill it again with option **4**.
