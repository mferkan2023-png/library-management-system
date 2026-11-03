// Database.cpp
// Here we write the SQL queries for insert, read, update, delete and search.
// We use prepared statements (sqlite3_prepare_v2) so user input is safe.

#include "Database.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>

using namespace std;

// small helper to get text from a column (SQLite can return NULL)
static string colText(sqlite3_stmt* st, int col) {
    const unsigned char* t = sqlite3_column_text(st, col);
    if (t == nullptr) return "";
    return string((const char*)t);
}

static Book readBook(sqlite3_stmt* st) {
    Book b;
    b.id = sqlite3_column_int(st, 0);
    b.title = colText(st, 1);
    b.author = colText(st, 2);
    b.category = colText(st, 3);
    b.year = sqlite3_column_int(st, 4);
    b.copies = sqlite3_column_int(st, 5);
    return b;
}

static Member readMember(sqlite3_stmt* st) {
    Member m;
    m.id = sqlite3_column_int(st, 0);
    m.name = colText(st, 1);
    m.phone = colText(st, 2);
    m.email = colText(st, 3);
    m.joinDate = colText(st, 4);
    return m;
}

Database::Database() {
    db = nullptr;
}

Database::~Database() {
    if (db != nullptr) sqlite3_close(db);
}

bool Database::open(const string& fileName) {
    if (sqlite3_open(fileName.c_str(), &db) != SQLITE_OK) {
        cout << "Error: cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

bool Database::runSQL(const string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        cout << "SQL error: " << err << endl;
        sqlite3_free(err);
        return false;
    }
    return true;
}

void Database::createTables() {
    runSQL("CREATE TABLE IF NOT EXISTS Books ("
           "book_id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "title TEXT NOT NULL,"
           "author TEXT NOT NULL,"
           "category TEXT,"
           "publish_year INTEGER,"
           "copies INTEGER DEFAULT 1);");

    runSQL("CREATE TABLE IF NOT EXISTS Members ("
           "member_id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "phone TEXT,"
           "email TEXT,"
           "join_date TEXT);");

    runSQL("CREATE TABLE IF NOT EXISTS Loans ("
           "loan_id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "book_id INTEGER NOT NULL,"
           "member_id INTEGER NOT NULL,"
           "issue_date TEXT NOT NULL,"
           "return_date TEXT,"
           "FOREIGN KEY(book_id) REFERENCES Books(book_id),"
           "FOREIGN KEY(member_id) REFERENCES Members(member_id));");

    // index on author column, we use it in the speed test
    runSQL("CREATE INDEX IF NOT EXISTS idx_books_author ON Books(author);");
}

int Database::countRows(const string& table) {
    sqlite3_stmt* st;
    string sql = "SELECT COUNT(*) FROM " + table + ";";
    int count = 0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr) == SQLITE_OK) {
        if (sqlite3_step(st) == SQLITE_ROW) count = sqlite3_column_int(st, 0);
    }
    sqlite3_finalize(st);
    return count;
}

void Database::beginTransaction() { runSQL("BEGIN TRANSACTION;"); }
void Database::commitTransaction() { runSQL("COMMIT;"); }

// ================= BOOKS =================

bool Database::addBook(const Book& b) {
    sqlite3_stmt* st;
    const char* sql = "INSERT INTO Books(title, author, category, publish_year, copies) VALUES(?,?,?,?,?);";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(st, 1, b.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, b.author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, b.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 4, b.year);
    sqlite3_bind_int(st, 5, b.copies);
    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);
    return ok;
}

vector<Book> Database::getAllBooks(int limit) {
    vector<Book> list;
    sqlite3_stmt* st;
    const char* sql = "SELECT * FROM Books ORDER BY book_id LIMIT ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, limit);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readBook(st));
    }
    sqlite3_finalize(st);
    return list;
}

bool Database::getBookById(int id, Book& b) {
    sqlite3_stmt* st;
    bool found = false;
    const char* sql = "SELECT * FROM Books WHERE book_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, id);
        if (sqlite3_step(st) == SQLITE_ROW) {
            b = readBook(st);
            found = true;
        }
    }
    sqlite3_finalize(st);
    return found;
}

vector<Book> Database::searchBooksByTitle(const string& text) {
    vector<Book> list;
    sqlite3_stmt* st;
    const char* sql = "SELECT * FROM Books WHERE title LIKE ?;";
    string pattern = "%" + text + "%";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readBook(st));
    }
    sqlite3_finalize(st);
    return list;
}

vector<Book> Database::searchBooksByAuthor(const string& author) {
    vector<Book> list;
    sqlite3_stmt* st;
    const char* sql = "SELECT * FROM Books WHERE author = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, author.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readBook(st));
    }
    sqlite3_finalize(st);
    return list;
}

vector<Book> Database::searchBooksByCategory(const string& category) {
    vector<Book> list;
    sqlite3_stmt* st;
    const char* sql = "SELECT * FROM Books WHERE category = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, category.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readBook(st));
    }
    sqlite3_finalize(st);
    return list;
}

bool Database::updateBook(const Book& b) {
    sqlite3_stmt* st;
    const char* sql = "UPDATE Books SET title=?, author=?, category=?, publish_year=?, copies=? WHERE book_id=?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(st, 1, b.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, b.author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, b.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 4, b.year);
    sqlite3_bind_int(st, 5, b.copies);
    sqlite3_bind_int(st, 6, b.id);
    bool ok = (sqlite3_step(st) == SQLITE_DONE) && sqlite3_changes(db) > 0;
    sqlite3_finalize(st);
    return ok;
}

bool Database::deleteBook(int id) {
    // first delete the loans of this book, then the book
    sqlite3_stmt* st;
    sqlite3_prepare_v2(db, "DELETE FROM Loans WHERE book_id = ?;", -1, &st, nullptr);
    sqlite3_bind_int(st, 1, id);
    sqlite3_step(st);
    sqlite3_finalize(st);

    sqlite3_prepare_v2(db, "DELETE FROM Books WHERE book_id = ?;", -1, &st, nullptr);
    sqlite3_bind_int(st, 1, id);
    sqlite3_step(st);
    bool ok = sqlite3_changes(db) > 0;
    sqlite3_finalize(st);
    return ok;
}

int Database::deleteBooksByAuthor(const string& author) {
    sqlite3_stmt* st;
    sqlite3_prepare_v2(db, "DELETE FROM Loans WHERE book_id IN (SELECT book_id FROM Books WHERE author = ?);", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(st);
    sqlite3_finalize(st);

    sqlite3_prepare_v2(db, "DELETE FROM Books WHERE author = ?;", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(st);
    int n = sqlite3_changes(db);
    sqlite3_finalize(st);
    return n;
}

// ================= MEMBERS =================

bool Database::addMember(const Member& m) {
    sqlite3_stmt* st;
    const char* sql = "INSERT INTO Members(name, phone, email, join_date) VALUES(?,?,?,?);";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(st, 1, m.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, m.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, m.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, m.joinDate.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);
    return ok;
}

vector<Member> Database::getAllMembers(int limit) {
    vector<Member> list;
    sqlite3_stmt* st;
    if (sqlite3_prepare_v2(db, "SELECT * FROM Members ORDER BY member_id LIMIT ?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, limit);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readMember(st));
    }
    sqlite3_finalize(st);
    return list;
}

bool Database::getMemberById(int id, Member& m) {
    sqlite3_stmt* st;
    bool found = false;
    if (sqlite3_prepare_v2(db, "SELECT * FROM Members WHERE member_id = ?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, id);
        if (sqlite3_step(st) == SQLITE_ROW) {
            m = readMember(st);
            found = true;
        }
    }
    sqlite3_finalize(st);
    return found;
}

vector<Member> Database::searchMembersByName(const string& text) {
    vector<Member> list;
    sqlite3_stmt* st;
    string pattern = "%" + text + "%";
    if (sqlite3_prepare_v2(db, "SELECT * FROM Members WHERE name LIKE ?;", -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(st, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(st) == SQLITE_ROW) list.push_back(readMember(st));
    }
    sqlite3_finalize(st);
    return list;
}

bool Database::updateMember(const Member& m) {
    sqlite3_stmt* st;
    const char* sql = "UPDATE Members SET name=?, phone=?, email=? WHERE member_id=?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(st, 1, m.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, m.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, m.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 4, m.id);
    bool ok = (sqlite3_step(st) == SQLITE_DONE) && sqlite3_changes(db) > 0;
    sqlite3_finalize(st);
    return ok;
}

bool Database::deleteMember(int id) {
    sqlite3_stmt* st;
    sqlite3_prepare_v2(db, "DELETE FROM Loans WHERE member_id = ?;", -1, &st, nullptr);
    sqlite3_bind_int(st, 1, id);
    sqlite3_step(st);
    sqlite3_finalize(st);

    sqlite3_prepare_v2(db, "DELETE FROM Members WHERE member_id = ?;", -1, &st, nullptr);
    sqlite3_bind_int(st, 1, id);
    sqlite3_step(st);
    bool ok = sqlite3_changes(db) > 0;
    sqlite3_finalize(st);
    return ok;
}

// ================= LOANS =================

bool Database::addLoan(const Loan& l) {
    sqlite3_stmt* st;
    const char* sql = "INSERT INTO Loans(book_id, member_id, issue_date, return_date) VALUES(?,?,?,?);";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(st, 1, l.bookId);
    sqlite3_bind_int(st, 2, l.memberId);
    sqlite3_bind_text(st, 3, l.issueDate.c_str(), -1, SQLITE_TRANSIENT);
    if (l.returnDate.empty()) sqlite3_bind_null(st, 4);
    else sqlite3_bind_text(st, 4, l.returnDate.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(st) == SQLITE_DONE);
    sqlite3_finalize(st);
    return ok;
}

vector<Loan> Database::getActiveLoans(int limit) {
    vector<Loan> list;
    sqlite3_stmt* st;
    const char* sql = "SELECT * FROM Loans WHERE return_date IS NULL ORDER BY loan_id LIMIT ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(st, 1, limit);
        while (sqlite3_step(st) == SQLITE_ROW) {
            Loan l;
            l.id = sqlite3_column_int(st, 0);
            l.bookId = sqlite3_column_int(st, 1);
            l.memberId = sqlite3_column_int(st, 2);
            l.issueDate = colText(st, 3);
            l.returnDate = colText(st, 4);
            list.push_back(l);
        }
    }
    sqlite3_finalize(st);
    return list;
}

bool Database::returnBook(int loanId, const string& date) {
    sqlite3_stmt* st;
    const char* sql = "UPDATE Loans SET return_date=? WHERE loan_id=? AND return_date IS NULL;";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(st, 1, date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, loanId);
    sqlite3_step(st);
    bool ok = sqlite3_changes(db) > 0;
    sqlite3_finalize(st);
    return ok;
}

// ================= SPEED TEST =================
// We run every search many times and take the average time.
// We also print the "query plan" so we can see if SQLite uses an index or scans the whole table.

static void printPlan(sqlite3* db, const string& sql) {
    sqlite3_stmt* st;
    string q = "EXPLAIN QUERY PLAN " + sql;
    if (sqlite3_prepare_v2(db, q.c_str(), -1, &st, nullptr) == SQLITE_OK) {
        while (sqlite3_step(st) == SQLITE_ROW) {
            cout << "      plan: " << colText(st, 3) << endl;
        }
    }
    sqlite3_finalize(st);
}

void Database::speedTest() {
    const int RUNS = 1000;
    int totalBooks = countRows("Books");
    if (totalBooks == 0) {
        cout << "No books in database. Please generate data first." << endl;
        return;
    }

    // take some real values from the table so every search finds something
    vector<Book> sample = getAllBooks(200);

    cout << "\nEach query runs " << RUNS << " times on " << totalBooks << " books.\n" << endl;

    // 1) search by ID (primary key)
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < RUNS; i++) {
        Book b;
        getBookById(sample[i % sample.size()].id, b);
    }
    auto t2 = chrono::high_resolution_clock::now();
    double timeId = chrono::duration<double, micro>(t2 - t1).count() / RUNS;

    // 2) search by author (column has an index)
    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < RUNS; i++) searchBooksByAuthor(sample[i % sample.size()].author);
    t2 = chrono::high_resolution_clock::now();
    double timeAuthor = chrono::duration<double, micro>(t2 - t1).count() / RUNS;

    // 3) search by category (no index)
    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < RUNS; i++) searchBooksByCategory(sample[i % sample.size()].category);
    t2 = chrono::high_resolution_clock::now();
    double timeCategory = chrono::duration<double, micro>(t2 - t1).count() / RUNS;

    // 4) search by part of title with LIKE (no index can help)
    string words[] = { "River", "Night", "Garden", "Secret", "Storm" };
    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < RUNS; i++) searchBooksByTitle(words[i % 5]);
    t2 = chrono::high_resolution_clock::now();
    double timeTitle = chrono::duration<double, micro>(t2 - t1).count() / RUNS;

    cout << fixed << setprecision(2) << right;
    cout << "1. Search by ID (primary key)      : " << setw(9) << timeId << " microseconds" << endl;
    printPlan(db, "SELECT * FROM Books WHERE book_id = 5;");
    cout << "2. Search by author (with index)   : " << setw(9) << timeAuthor << " microseconds" << endl;
    printPlan(db, "SELECT * FROM Books WHERE author = 'x';");
    cout << "3. Search by category (no index)   : " << setw(9) << timeCategory << " microseconds" << endl;
    printPlan(db, "SELECT * FROM Books WHERE category = 'x';");
    cout << "4. Search by title LIKE '%word%'   : " << setw(9) << timeTitle << " microseconds" << endl;
    printPlan(db, "SELECT * FROM Books WHERE title LIKE '%x%';");

    cout << "\nNote: SEARCH = SQLite uses an index. SCAN = SQLite reads every row." << endl;
}
