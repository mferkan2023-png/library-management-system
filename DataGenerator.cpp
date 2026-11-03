// DataGenerator.cpp
// We did not want to type 2000 records by hand, so this file makes them randomly.
// Book titles and names are made by mixing words from small lists.

#include "DataGenerator.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

static string firstNames[] = { "Ali", "Ahmed", "Fatima", "Ayesha", "Usman", "Hamza", "Zainab", "Bilal",
                               "Sara", "Omar", "Hira", "Fatih", "Emre", "Maryam", "Hassan", "Iqra",
                               "John", "Emma", "David", "Sophia", "Daniel", "Olivia", "Yusuf", "Elif" };

static string lastNames[] = { "Khan", "Ahmed", "Malik", "Hussain", "Raza", "Sheikh", "Butt", "Iqbal",
                              "Yilmaz", "Kaya", "Demir", "Smith", "Brown", "Wilson", "Taylor", "Clark",
                              "Qureshi", "Chaudhry", "Aslam", "Javed" };

static string adjectives[] = { "Silent", "Lost", "Hidden", "Broken", "Golden", "Dark", "Little", "Last",
                               "Forgotten", "Red", "Endless", "Quiet", "Wild", "Secret", "Old", "Blue" };

static string nouns[] = { "River", "Night", "Garden", "City", "Storm", "Mountain", "Letter", "Kingdom",
                          "Island", "Road", "House", "Dream", "Forest", "Sky", "Door", "Promise" };

static string extras[] = { "", "", "", " of Time", " and Other Stories", " in Winter", " Returns", ": Part 2" };

static string categories[] = { "Fiction", "Science", "History", "Computer", "Mathematics",
                               "Biography", "Poetry", "Children", "Religion", "Business" };

static int randomInt(int low, int high) {
    return low + rand() % (high - low + 1);
}

static string makeDate(int year) {
    int month = randomInt(1, 12);
    int day = randomInt(1, 28);
    string m = (month < 10 ? "0" : "") + to_string(month);
    string d = (day < 10 ? "0" : "") + to_string(day);
    return to_string(year) + "-" + m + "-" + d;
}

void generateData(Database& db) {
    if (db.countRows("Books") > 0) {
        cout << "Database already has data. Generation skipped." << endl;
        return;
    }

    srand(42);   // fixed seed, so we get the same data every time
    cout << "Generating data, please wait..." << endl;

    // one transaction makes thousands of inserts much faster
    db.beginTransaction();

    for (int i = 0; i < 2000; i++) {
        Book b;
        b.title = "The " + adjectives[rand() % 16] + " " + nouns[rand() % 16] + extras[rand() % 8];
        b.author = firstNames[rand() % 24] + " " + lastNames[rand() % 20];
        b.category = categories[rand() % 10];
        b.year = randomInt(1950, 2025);
        b.copies = randomInt(1, 10);
        db.addBook(b);
    }

    for (int i = 0; i < 500; i++) {
        Member m;
        string first = firstNames[rand() % 24];
        string last = lastNames[rand() % 20];
        m.name = first + " " + last;
        m.phone = "03" + to_string(randomInt(10, 49)) + "-" + to_string(randomInt(1000000, 9999999));
        m.email = first + "." + last + to_string(i + 1) + "@mail.com";
        m.joinDate = makeDate(randomInt(2020, 2025));
        db.addMember(m);
    }

    for (int i = 0; i < 1000; i++) {
        Loan l;
        l.bookId = randomInt(1, 2000);
        l.memberId = randomInt(1, 500);
        l.issueDate = makeDate(2025);
        // about 70% of the books are already returned
        if (rand() % 10 < 7) l.returnDate = makeDate(2026);
        db.addLoan(l);
    }

    db.commitTransaction();

    cout << "Done! Books: " << db.countRows("Books")
         << ", Members: " << db.countRows("Members")
         << ", Loans: " << db.countRows("Loans") << endl;
}
