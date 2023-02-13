//
// Created by Godfrey Bafana on 2023/02/09.
//

#ifndef TASKMANAGERTERMINAL_DATASOURCELAYER_H
#define TASKMANAGERTERMINAL_DATASOURCELAYER_H

#include <iostream>
#include "sqlite3.h"

class DataSourceLayer
{
public:
    DataSourceLayer() : db_(nullptr) { Connect("db/tasks.db"); }
    // Connects to the database specified by 'db_file'
    bool Connect(const std::string &);
    // Closes the connection to the database
    void Close();
    // Executes the SQL statement 'sql'
    bool ExecuteSQL(const std::string &);
    bool ExecuteSQL(sqlite3_stmt *);
    // Executes the SQL query 'sql' and retrieves the results
    bool GetResults(const std::string &,int (*)(void*, int, char**, char**),void *);
    // Prepares an SQL statement for execution
    bool PrepareStatement(const std::string &, sqlite3_stmt **);
    void seedDatabase();

private:
    sqlite3 *db_;
};

#endif // TASKMANAGERTERMINAL_DATASOURCELAYER_H
