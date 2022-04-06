#include <iostream>
#include <string>
#include <cstdint>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <fstream>

#define LOGIN    0
#define REGISTER 1

static int32_t action = REGISTER;
static const std::string dataBaseFileName = "passwords.db";

static void PrintHelp( char* progName );
static int ParseCmd( int argc, char* argv[] );
static void SignUpNewUser();
static int GetUserPassword( std::string& passwd );
static sqlite3* OpenDataBase( const std::string& dbName );
static int AddToDataBase( sqlite3* db, const std::string& siteAddr, const std::string& userName,
                          std::string& passwd );
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main( int argc, char* argv[] )
{
    if( ParseCmd( argc, argv ) )
    {
        return EXIT_FAILURE;
    }
    if( action == REGISTER )
    {
        SignUpNewUser();
    }
}

void SignUpNewUser()
{
    std::string siteAddr;
    std::cout << "Input Site addres: ";
    std::cin >> siteAddr;
    std::string userName;
    std::cout << "Input User name: ";
    std::cin >> userName;
    std::cout << "Input User password: ";
    std::string userPasswd;
    if( GetUserPassword( userPasswd ) )
    {
        exit( 1 );
    }

    sqlite3* passwordsBase = nullptr;
    passwordsBase = OpenDataBase( dataBaseFileName );
    if( !passwordsBase )
    {
        std::cerr << "Can't sugn up new user." << std::endl;
    }
    if( AddToDataBase( passwordsBase, siteAddr, userName, userPasswd ) )
    {
        std::cerr << "Error adding to database." << std::endl;
    }

}

sqlite3* OpenDataBase( const std::string& dbName )
{
    sqlite3* db;
    bool isNew = false;
    std::fstream dbFile( dbName );
    if( !dbFile.is_open() )
    {
        isNew = true;
        dbFile.open( dbName );
        dbFile.close();
    }

    dbFile.seekg( 0, std::ios_base::end );
    if( 0 == dbFile.tellg() )
    {
        isNew = true;
    }
    dbFile.close();

    int res = sqlite3_open( dbName.c_str(), &db );

    if( res )
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg( db ) << std::endl;
    }
    else
    {
        std::cout << "SECRET database opened successfully" << std::endl;
    }

    if( isNew )
    {
        char* errMsg;
        std::cout << "It was new file" << std::endl;
        std::string sqlCreateReq( "CREATE TABLE PASSWORDS("  \
                                  "SITE CHAR(50) PRIMARY KEY," \
                                  "LOGIN CHAR(50)," \
                                  "PASSWORD CHAR(50) );");
        if( SQLITE_OK != sqlite3_exec( db, sqlCreateReq.c_str(), callback, 0, &errMsg ) )
        {
            std::cerr << "Problems with req exec" << std::endl
                      << "SQL error: " << errMsg << std::endl;
            sqlite3_free( errMsg );
            return nullptr;
        }
    }
    return db;
}

int AddToDataBase( sqlite3* db, const std::string& siteAddr, const std::string& userName,
                   std::string& passwd )
{
    char* errMsg;
    std::string sqlInsertReq("INSERT INTO PASSWORDS (SITE,LOGIN,PASSWORD) \n");
    sqlInsertReq += "VALUES ('" + siteAddr + "', '" + userName + "', '" + passwd + "' ); ";
    std::cout << sqlInsertReq << std::endl;
    if( SQLITE_OK != sqlite3_exec( db, sqlInsertReq.c_str(), callback, 0, &errMsg ) )
    {
        std::cerr << "Problems with req exec" << std::endl
                  << "SQL error: " << errMsg << std::endl;
        sqlite3_free( errMsg );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int GetUserPassword( std::string& userPasswd )
{
    userPasswd = getpass( "" );
    return EXIT_SUCCESS;
}

void PrintHelp( char* progName )
{
    std::cout << "Usage: " << progName << std::endl
              << "\t-l  flag   Check login" << std::endl
              << "\t-r  flag   Register"    << std::endl;
}

int ParseCmd( int argc, char* argv[] )
{
    int opt = 0;
    if ( argc == 1 )
    {
        PrintHelp( argv[ 0 ] );
        return EXIT_FAILURE;
    }
    while( -1 != ( opt = getopt( argc, argv, ":rl")) )
    {
        switch ( opt )
        {
        case 'r':
            action = REGISTER;
            return EXIT_SUCCESS;
        case 'l':
            action = LOGIN;
            return EXIT_SUCCESS;
        default:
            PrintHelp( argv[ 0 ] );
            return EXIT_FAILURE;
        }
    }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}