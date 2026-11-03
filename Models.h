// Models.h
// These classes hold the data of one row from each table.

#pragma once
#include <string>

class Book {
public:
    int id = 0;
    std::string title;
    std::string author;
    std::string category;
    int year = 0;
    int copies = 0;
};

class Member {
public:
    int id = 0;
    std::string name;
    std::string phone;
    std::string email;
    std::string joinDate;
};

class Loan {
public:
    int id = 0;
    int bookId = 0;
    int memberId = 0;
    std::string issueDate;
    std::string returnDate;   // empty means book is not returned yet
};
