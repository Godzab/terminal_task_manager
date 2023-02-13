//
// Created by Godfrey Bafana on 2023/02/09.
//
#include <iostream>
#include "vector"
#include "tuple"
#include "../include/DataSourceLayer.h"
#include "../include/BusinessLogicLayer.h"

using namespace std;

// Connects to the database specified by 'db_file'
bool DataSourceLayer::Connect(const std::string &db_file)
{
    int result = sqlite3_open(db_file.c_str(), &db_);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db_)
                  << std::endl;
        return false;
    }
    return true;
}

// Closes the connection to the database
void DataSourceLayer::Close()
{
    if (db_ != nullptr)
    {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

// Executes the SQL query 'sql' and retrieves the results
bool DataSourceLayer::GetResults(const std::string &sql,
                                 int (*callback)(void *, int, char **, char **),
                                 void *arg)
{
    char *error_message = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), callback, arg, &error_message);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error executing SQL: " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    return true;
}

void DataSourceLayer::seedDatabase() {
    // Define the seed data as a vector of tuples
    std::vector<std::tuple<std::string, std::string, int, std::string, std::string>> data{
        tuple<std::string, std::string, int, std::string, std::string>{"Write Report", "Create a report on the sales data of the first quarter",
                                                                       static_cast<const int>(TaskStatus::TODO), "2023-02-08 10:00:00", "To provide insights into the sales performance for the first quarter"},
        tuple<std::string, std::string, int, std::string, std::string>{"Organize Meeting", "Arrange a meeting with the team to discuss the project plan",
                                                                       static_cast<const int>(TaskStatus::TODO), "2023-02-08 11:00:00", "To align everyone on the project plan and next steps"},
        tuple<std::string, std::string, int, std::string, std::string>{"Design Website", "Design a new website for the company",
                                                                       static_cast<const int>(TaskStatus::IN_PROGRESS), "2023-02-08 11:00:00", "To improve the online presence and attract more customers"},
        tuple<std::string, std::string, int, std::string, std::string>{"Develop Mobile App", "Develop a mobile app for the company",
                                                                       static_cast<const int>(TaskStatus::DONE), "2023-02-08 12:00:00", "To provide customers with a convenient way to interact with the company"},
    };

    // Prepare the SQL statement for insertion
    sqlite3_stmt *stmt;
    if (!PrepareStatement("INSERT INTO tasks (name, description, status, created_at, objective) VALUES (?, ?, ?, ?, ?)", &stmt)) {
        std::cerr << "Error preparing SQL statement." << std::endl;
        // Handle the error
        return;
    }

    // Insert the seed data into the tasks table
    for (const auto &item : data) {
        // Bind the parameter values to the prepared statement
        std::string name = std::get<0>(item);
        std::string description = std::get<1>(item);
        int status = std::get<2>(item);
        std::string created_at = std::get<3>(item);
        std::string objective = std::get<4>(item);

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, status);
        sqlite3_bind_text(stmt, 4, created_at.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, objective.c_str(), -1, SQLITE_STATIC);

        // Execute the prepared statement
        if (!ExecuteSQL(stmt)) {
            std::cerr << "Error inserting data into tasks table." << std::endl;
            // Handle the error
            return;
        }

        // Reset the prepared statement for reuse
        sqlite3_reset(stmt);
    }

    // Clean up
    sqlite3_finalize(stmt);
}

bool DataSourceLayer::ExecuteSQL(const std::string &sql) {
    if (!db_) {
        std::cerr << "Error: database is not connected." << std::endl;
        return false;
    }

    char *errmsg;
    int ret = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (ret != SQLITE_OK) {
        std::cerr << "Error executing SQL statement: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }

    return true;
}

// Definition for sqlite3_stmt argument
bool DataSourceLayer::ExecuteSQL(sqlite3_stmt *stmt) {
    if (!db_) {
        std::cerr << "Error: database is not connected." << std::endl;
        return false;
    }

    int ret = sqlite3_step(stmt);
    if (ret != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_reset(stmt);
    return true;
}

bool DataSourceLayer::PrepareStatement(const std::string &query, sqlite3_stmt **stmt) {
    int ret = sqlite3_prepare_v2(db_, query.c_str(), -1, stmt, nullptr);
    if (ret != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    return true;
}
