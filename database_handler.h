#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H

#include <unistd.h>
#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <commands.h>

sqlite3* OpenDataBase( const std::string& dbName );
int InsertToDataBase( sqlite3* db, const std::string& siteAddr, const std::string& userName,
                          std::string& passwd );
int UpdatePassword( sqlite3* db, const std::string& siteAddr );
int SelectFromDataBase( sqlite3* db, const std::string& siteAddr );
int ListDataBaseSites( sqlite3* db );
int callback( void *NotUsed, int argc, char **argv, char **azColName );

const std::string dataBaseFileName = "passwords.db";

#endif