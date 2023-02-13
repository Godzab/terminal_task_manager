//
// Created by Godfrey Bafana on 2023/02/09.
//
#include <iostream>
#include "../include/DataSourceLayer.h"

using namespace std;

// Connects to the database specified by 'db_file'
bool DataSourceLayer::Connect(const std::string &db_file) {
    int result = sqlite3_open(db_file.c_str(), &db_);
    if (result != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db_)
                  << std::endl;
        return false;
    }
    return true;
}

// Closes the connection to the database
void DataSourceLayer::Close() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

// Executes the SQL statement 'sql'
bool DataSourceLayer::ExecuteSQL(const std::string &sql) {
    char *error_message = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error_message);
    if (result != SQLITE_OK) {
        std::cerr << "Error executing SQL: " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    return true;
}

// Executes the SQL query 'sql' and retrieves the results
bool DataSourceLayer::GetResults(const std::string &sql,
                int (*callback)(void*, int, char**, char**),
                void *arg) {
    char *error_message = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), callback, arg, &error_message);
    if (result != SQLITE_OK) {
        std::cerr << "Error executing SQL: " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    return true;
}

void DataSourceLayer::seedDatabase() {
    // SQL statement to create the tasks table
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS tasks "
                                 "(name TEXT NOT NULL, "
                                 "description TEXT NOT NULL, "
                                 "status INTEGER NOT NULL, "
                                 "created_at DATETIME NOT NULL, "
                                 "objective TEXT NOT NULL);";

    // Execute the create table SQL statement
    int createTableResult = sqlite3_exec(db_, createTableSQL, 0, 0, 0);

    // Check if the create table SQL statement was executed successfully
    if (createTableResult != SQLITE_OK) {
        cerr << "Error creating tasks table: " << sqlite3_errmsg(db_) << endl;
        return;
    }

    // Sample data to seed the tasks table
    string seedDataSQL = "INSERT INTO tasks (name, description, status, created_at, objective)"
                         " VALUES ('Write Report', 'Create a report on the sales data of the first quarter', 0, '2023-02-08 10:00:00', 'To provide insights into the sales performance for the first quarter'),"
                         "('Organize Meeting', 'Arrange a meeting with the team to discuss the project plan', 0, '2023-02-08 11:00:00', 'To align everyone on the project plan and next steps'),"
                         "('Design Website', 'Design a new website for the company', 2, '2023-02-08 11:00:00', 'To improve the online presence and attract more customers'),"
                         "('Develop Mobile App', 'Develop a mobile app for the company', 3, '2023-02-08 12:00:00', 'To provide customers with a convenient way to interact with the company');";

    // Execute the seed data SQL statement
    int seedDataResult = sqlite3_exec(db_, seedDataSQL.c_str(), 0, 0, 0);

    // Check if the seed data SQL statement was executed successfully
    if (seedDataResult != SQLITE_OK) {
        cerr << "Error seeding tasks table: " << sqlite3_errmsg(db_) << endl;
        return;
    }

    cout << "Tasks table seeded successfully." << endl;
}