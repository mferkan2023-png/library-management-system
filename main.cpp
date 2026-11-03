// main.cpp
// Library Management System - ICT Lab Semester Project (Fall 2026)
// Console menu for the user. All database work is in the Database class.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Database.h"
#include "DataGenerator.h"

using namespace std;

Database db;

// ---------- input helpers ----------

int readInt(const string& msg) {
    string line;
    while (true) {
        cout << msg;
        getline(cin, line);
        try {
            size_t pos;
            int value = stoi(line, &pos);
            if (pos == line.size()) return value;
        }
        catch (...) {}
        cout << "Please enter a number." << endl;
    }
}

string readText(const string& msg, bool canBeEmpty = false) {
    string line;
    while (true) {
        cout << msg;
        getline(cin, line);
        if (!line.empty() || canBeEmpty) return line;
        cout << "This field cannot be empty." << endl;
    }
}

void pause() {
    cout << "\nPress Enter to continue...";
    string tmp;
    getline(cin, tmp);
}

// ---------- print functions ----------

void printBooks(const vector<Book>& books) {
    if (books.empty()) {
        cout << "No book found." << endl;
        return;
    }
    cout << left << setw(6) << "ID" << setw(40) << "Title" << setw(20) << "Author"
         << setw(13) << "Category" << setw(6) << "Year" << "Copies" << endl;
    cout << string(91, '-') << endl;
    for (const Book& b : books) {
        cout << left << setw(6) << b.id << setw(40) << b.title.substr(0, 39) << setw(20) << b.author.substr(0, 19)
             << setw(13) << b.category << setw(6) << b.year << b.copies << endl;
    }
    cout << books.size() << " book(s)." << endl;
}

void printMembers(const vector<Member>& members) {
    if (members.empty()) {
        cout << "No member found." << endl;
        return;
    }
    cout << left << setw(6) << "ID" << setw(22) << "Name" << setw(15) << "Phone"
         << setw(32) << "Email" << "Joined" << endl;
    cout << string(87, '-') << endl;
    for (const Member& m : members) {
        cout << left << setw(6) << m.id << setw(22) << m.name << setw(15) << m.phone
             << setw(32) << m.email << m.joinDate << endl;
    }
    cout << members.size() << " member(s)." << endl;
}

// ---------- books menu ----------

void addBook() {
    Book b;
    b.title = readText("Title: ");
    b.author = readText("Author: ");
    b.category = readText("Category: ");
    b.year = readInt("Publish year: ");
    b.copies = readInt("Number of copies: ");
    if (db.addBook(b)) cout << "Book saved." << endl;
    else cout << "Error: book not saved." << endl;
}

void searchBooks() {
    cout << "\nSearch by: 1) ID  2) Title  3) Author  4) Category" << endl;
    int c = readInt("Choice: ");
    if (c == 1) {
        Book b;
        if (db.getBookById(readInt("Book ID: "), b)) printBooks({ b });
        else cout << "No book with this ID." << endl;
    }
    else if (c == 2) printBooks(db.searchBooksByTitle(readText("Part of title: ")));
    else if (c == 3) printBooks(db.searchBooksByAuthor(readText("Author full name: ")));
    else if (c == 4) printBooks(db.searchBooksByCategory(readText("Category: ")));
    else cout << "Wrong choice." << endl;
}

void updateBook() {
    Book b;
    int id = readInt("Book ID to update: ");
    if (!db.getBookById(id, b)) {
        cout << "No book with this ID." << endl;
        return;
    }
    printBooks({ b });
    cout << "Press Enter to keep the old value." << endl;

    string s = readText("New title: ", true);
    if (!s.empty()) b.title = s;
    s = readText("New author: ", true);
    if (!s.empty()) b.author = s;
    s = readText("New category: ", true);
    if (!s.empty()) b.category = s;
    s = readText("New year: ", true);
    if (!s.empty()) b.year = stoi(s);
    s = readText("New copies: ", true);
    if (!s.empty()) b.copies = stoi(s);

    if (db.updateBook(b)) cout << "Book updated." << endl;
    else cout << "Error: book not updated." << endl;
}

void deleteBook() {
    cout << "\nDelete by: 1) ID  2) Author" << endl;
    int c = readInt("Choice: ");
    if (c == 1) {
        Book b;
        int id = readInt("Book ID: ");
        if (!db.getBookById(id, b)) {
            cout << "No book with this ID." << endl;
            return;
        }
        printBooks({ b });
        string ans = readText("Are you sure? (y/n): ");
        if (ans == "y" || ans == "Y") {
            if (db.deleteBook(id)) cout << "Book deleted." << endl;
        }
    }
    else if (c == 2) {
        string author = readText("Author full name: ");
        vector<Book> list = db.searchBooksByAuthor(author);
        printBooks(list);
        if (list.empty()) return;
        string ans = readText("Delete all these books? (y/n): ");
        if (ans == "y" || ans == "Y") {
            cout << db.deleteBooksByAuthor(author) << " book(s) deleted." << endl;
        }
    }
}

void booksMenu() {
    while (true) {
        cout << "\n===== BOOKS =====" << endl;
        cout << "1. Add book\n2. View books\n3. Search book\n4. Update book\n5. Delete book\n0. Back" << endl;
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) addBook();
        else if (c == 2) printBooks(db.getAllBooks(readInt("How many books to show? ")));
        else if (c == 3) searchBooks();
        else if (c == 4) updateBook();
        else if (c == 5) deleteBook();
        else cout << "Wrong choice." << endl;
        pause();
    }
}

// ---------- members menu ----------

void addMember() {
    Member m;
    m.name = readText("Name: ");
    m.phone = readText("Phone: ");
    m.email = readText("Email: ");
    m.joinDate = readText("Join date (YYYY-MM-DD): ");
    if (db.addMember(m)) cout << "Member saved." << endl;
    else cout << "Error: member not saved." << endl;
}

void searchMembers() {
    cout << "\nSearch by: 1) ID  2) Name" << endl;
    int c = readInt("Choice: ");
    if (c == 1) {
        Member m;
        if (db.getMemberById(readInt("Member ID: "), m)) printMembers({ m });
        else cout << "No member with this ID." << endl;
    }
    else if (c == 2) printMembers(db.searchMembersByName(readText("Part of name: ")));
    else cout << "Wrong choice." << endl;
}

void updateMember() {
    Member m;
    int id = readInt("Member ID to update: ");
    if (!db.getMemberById(id, m)) {
        cout << "No member with this ID." << endl;
        return;
    }
    printMembers({ m });
    cout << "Press Enter to keep the old value." << endl;
    string s = readText("New name: ", true);
    if (!s.empty()) m.name = s;
    s = readText("New phone: ", true);
    if (!s.empty()) m.phone = s;
    s = readText("New email: ", true);
    if (!s.empty()) m.email = s;
    if (db.updateMember(m)) cout << "Member updated." << endl;
    else cout << "Error: member not updated." << endl;
}

void deleteMember() {
    Member m;
    int id = readInt("Member ID to delete: ");
    if (!db.getMemberById(id, m)) {
        cout << "No member with this ID." << endl;
        return;
    }
    printMembers({ m });
    string ans = readText("Are you sure? (y/n): ");
    if (ans == "y" || ans == "Y") {
        if (db.deleteMember(id)) cout << "Member deleted." << endl;
    }
}

void membersMenu() {
    while (true) {
        cout << "\n===== MEMBERS =====" << endl;
        cout << "1. Add member\n2. View members\n3. Search member\n4. Update member\n5. Delete member\n0. Back" << endl;
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) addMember();
        else if (c == 2) printMembers(db.getAllMembers(readInt("How many members to show? ")));
        else if (c == 3) searchMembers();
        else if (c == 4) updateMember();
        else if (c == 5) deleteMember();
        else cout << "Wrong choice." << endl;
        pause();
    }
}

// ---------- loans menu ----------

void issueBook() {
    Loan l;
    Book b;
    Member m;
    l.bookId = readInt("Book ID: ");
    if (!db.getBookById(l.bookId, b)) {
        cout << "No book with this ID." << endl;
        return;
    }
    l.memberId = readInt("Member ID: ");
    if (!db.getMemberById(l.memberId, m)) {
        cout << "No member with this ID." << endl;
        return;
    }
    l.issueDate = readText("Issue date (YYYY-MM-DD): ");
    if (db.addLoan(l)) cout << "\"" << b.title << "\" is issued to " << m.name << "." << endl;
}

void loansMenu() {
    while (true) {
        cout << "\n===== LOANS =====" << endl;
        cout << "1. Issue book\n2. Return book\n3. View books not returned\n0. Back" << endl;
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) issueBook();
        else if (c == 2) {
            int id = readInt("Loan ID: ");
            string date = readText("Return date (YYYY-MM-DD): ");
            if (db.returnBook(id, date)) cout << "Book returned." << endl;
            else cout << "Loan not found or already returned." << endl;
        }
        else if (c == 3) {
            vector<Loan> list = db.getActiveLoans(readInt("How many to show? "));
            cout << left << setw(8) << "LoanID" << setw(8) << "BookID" << setw(10) << "MemberID" << "Issue date" << endl;
            cout << string(38, '-') << endl;
            for (const Loan& l : list)
                cout << left << setw(8) << l.id << setw(8) << l.bookId << setw(10) << l.memberId << l.issueDate << endl;
        }
        else cout << "Wrong choice." << endl;
        pause();
    }
}

// ---------- main ----------

int main() {
    if (!db.open("library.db")) return 1;
    db.createTables();

    while (true) {
        cout << "\n=================================" << endl;
        cout << "    LIBRARY MANAGEMENT SYSTEM" << endl;
        cout << "=================================" << endl;
        cout << "1. Books" << endl;
        cout << "2. Members" << endl;
        cout << "3. Loans (issue / return)" << endl;
        cout << "4. Generate sample data (2000+ records)" << endl;
        cout << "5. Search speed test" << endl;
        cout << "6. Show number of records" << endl;
        cout << "0. Exit" << endl;

        int c = readInt("Choice: ");
        if (c == 0) break;
        if (c == 1) booksMenu();
        else if (c == 2) membersMenu();
        else if (c == 3) loansMenu();
        else if (c == 4) { generateData(db); pause(); }
        else if (c == 5) { db.speedTest(); pause(); }
        else if (c == 6) {
            int b = db.countRows("Books"), m = db.countRows("Members"), l = db.countRows("Loans");
            cout << "Books: " << b << "\nMembers: " << m << "\nLoans: " << l
                 << "\nTotal: " << b + m + l << endl;
            pause();
        }
        else cout << "Wrong choice." << endl;
    }

    cout << "Goodbye!" << endl;
    return 0;
}
