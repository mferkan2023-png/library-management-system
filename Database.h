// Database.h
// Database class talks with SQLite. All SQL code is inside this class.

#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"
#include "Models.h"

class Database {
private:
    sqlite3* db;
    bool runSQL(const std::string& sql);

public:
    Database();
    ~Database();

    bool open(const std::string& fileName);
    void createTables();
    int countRows(const std::string& table);
    void beginTransaction();
    void commitTransaction();

    // Books
    bool addBook(const Book& b);
    std::vector<Book> getAllBooks(int limit);
    bool getBookById(int id, Book& b);
    std::vector<Book> searchBooksByTitle(const std::string& text);
    std::vector<Book> searchBooksByAuthor(const std::string& author);
    std::vector<Book> searchBooksByCategory(const std::string& category);
    bool updateBook(const Book& b);
    bool deleteBook(int id);
    int deleteBooksByAuthor(const std::string& author);

    // Members
    bool addMember(const Member& m);
    std::vector<Member> getAllMembers(int limit);
    bool getMemberById(int id, Member& m);
    std::vector<Member> searchMembersByName(const std::string& text);
    bool updateMember(const Member& m);
    bool deleteMember(int id);

    // Loans
    bool addLoan(const Loan& l);
    std::vector<Loan> getActiveLoans(int limit);
    bool returnBook(int loanId, const std::string& date);

    // Speed test
    void speedTest();
};
