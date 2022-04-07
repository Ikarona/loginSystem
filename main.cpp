#include <iostream>
#include <string>
#include <cstdint>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <fstream>

#define GET_PASSWORD          0
#define REGISTER              1
#define CHANGE_PASSWORD       3
#define LIST_REGISTERED_SITES 4

static int32_t action = REGISTER;
static const std::string dataBaseFileName = "passwords.db";

static void AddNewPasswordToBase();
static void GetSitePassword();
static void PrintAllRegisteredSites();
static void ChangePassword();

static void PrintHelp( char* progName );
static int ParseCmd( int argc, char* argv[] );
static int GetUserPassword( std::string& passwd );
static sqlite3* OpenDataBase( const std::string& dbName );
static int InsertToDataBase( sqlite3* db, const std::string& siteAddr, const std::string& userName,
                          std::string& passwd );
static int UpdatePassword( sqlite3* db, const std::string& siteAddr );

static int SelectFromDataBase( sqlite3* db, const std::string& siteAddr );
static int ListDataBaseSites( sqlite3* db );

static int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main( int argc, char* argv[] )
{
    if( ParseCmd( argc, argv ) )
    {
        return EXIT_FAILURE;
    }

    switch ( action )
    {
    case REGISTER:
        AddNewPasswordToBase();
        break;
    case GET_PASSWORD:
        GetSitePassword();
        break;
    case LIST_REGISTERED_SITES:
        PrintAllRegisteredSites();
        break;
    case CHANGE_PASSWORD:
        ChangePassword();
        break;
    default:
        break;
    }
}

void AddNewPasswordToBase()
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
    if( InsertToDataBase( passwordsBase, siteAddr, userName, userPasswd ) )
    {
        std::cerr << "Error adding to database." << std::endl;
    }
    sqlite3_close( passwordsBase );
}

void GetSitePassword()
{
    std::string siteAddr;
    std::cout << "Input Site addres: ";
    std::cin >> siteAddr;
    sqlite3* passwordsBase = nullptr;
    passwordsBase = OpenDataBase( dataBaseFileName );
    if( !passwordsBase )
    {
        std::cerr << "Can't sugn up new user." << std::endl;
    }

    if( SelectFromDataBase( passwordsBase, siteAddr ) )
    {
        std::cerr << "Error selecting from database." << std::endl;
    }
    sqlite3_close( passwordsBase );
}

void PrintAllRegisteredSites()
{
    sqlite3* passwordsBase = nullptr;
    passwordsBase = OpenDataBase( dataBaseFileName );
    if( !passwordsBase )
    {
        std::cerr << "Can't sugn up new user." << std::endl;
    }

    std::cout << "List of all sites: " << std::endl;
    if( ListDataBaseSites( passwordsBase ) )
    {
        std::cerr << "Failed to get list of all sites." << std::endl;
    }
}

void ChangePassword()
{
    std::string siteAddr;
    std::cout << "Input Site addres: ";
    std::cin >> siteAddr;
    sqlite3* passwordsBase = nullptr;
    passwordsBase = OpenDataBase( dataBaseFileName );
    if( !passwordsBase )
    {
        std::cerr << "Can't sugn up new user." << std::endl;
    }
    if( UpdatePassword( passwordsBase, siteAddr ) )
    {
        std::cerr << "Can't update password." << std::endl;
    }
}

int InsertToDataBase( sqlite3* db, const std::string& siteAddr, const std::string& userName,
                      std::string& passwd )
{
    char* errMsg;
    std::string sqlInsertReq( "INSERT INTO PASSWORDS (SITE,LOGIN,PASSWORD) \n");
    sqlInsertReq += "VALUES ('" + siteAddr + "', '" + userName + "', '" + passwd + "' ); ";
    passwd.clear();
    if( SQLITE_OK != sqlite3_exec( db, sqlInsertReq.c_str(), callback, 0, &errMsg ) )
    {
        std::cerr << "Problems with req exec" << std::endl
                  << "SQL error: " << errMsg << std::endl;
        sqlite3_free( errMsg );
        return EXIT_FAILURE;
    }
    sqlInsertReq.clear();
    return EXIT_SUCCESS;
}

int UpdatePassword( sqlite3* db, const std::string& siteAddr )
{
    std::string sqlUpdateReq( "SELECT * FROM PASSWORDS WHERE SITE = " );
    sqlUpdateReq += "'" + siteAddr + "'";
    sqlite3_stmt *pStmt = NULL;
    if( SQLITE_OK != sqlite3_prepare_v2( db, sqlUpdateReq.c_str(), sqlUpdateReq.length(), &pStmt, NULL ) )
    {
        std::cerr << "Problems with req exec" << std::endl;
        return EXIT_SUCCESS;
    }

    int step = sqlite3_step( pStmt );
    std::string siteName;
    std::string userName;
    std::string userPass;
    if( step == SQLITE_ROW )
    {
        siteName = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 0 ) ) );
        userName = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 1 ) ) );
        userPass = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 2 ) ) );
    }

    if( !siteName.empty() && !userName.empty() && !userPass.empty() )
    {
        std::cout << "Site name: " << siteName << std::endl;
        std::cout << "User name: " << userName << std::endl;
    }
    else
    {
        std::cerr << "No such record in database." << std::endl;
    }

    std::string newPasswd;
    std::cout << "Enter new password: ";
    GetUserPassword( newPasswd );
    char* errMsg;
    sqlUpdateReq = "UPDATE PASSWORDS set PASSWORD='" + newPasswd + "' where SITE = '" + siteAddr + "';";
    if( SQLITE_OK != sqlite3_exec( db, sqlUpdateReq.c_str(), callback, 0, &errMsg ) )
    {
        std::cerr << "Problems with req exec" << std::endl
                    << "SQL error: " << errMsg << std::endl;
        sqlite3_free( errMsg );
        return EXIT_FAILURE;
    }
    sqlite3_finalize( pStmt );
    return EXIT_SUCCESS;

}

int SelectFromDataBase( sqlite3* db, const std::string& siteAddr )
{
    std::string sqlSelectReq( "SELECT * FROM PASSWORDS WHERE SITE = " );
    sqlSelectReq += "'" + siteAddr + "'";
    sqlite3_stmt *pStmt = NULL;
    if( SQLITE_OK != sqlite3_prepare_v2( db, sqlSelectReq.c_str(), sqlSelectReq.length(), &pStmt, NULL ) )
    {
        std::cerr << "Problems with req exec" << std::endl;
        return EXIT_SUCCESS;
    }

    int step = sqlite3_step( pStmt );
    std::string siteName;
    std::string userName;
    std::string userPass;
    if( step == SQLITE_ROW )
    {
        siteName = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 0 ) ) );
        userName = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 1 ) ) );
        userPass = std::string( reinterpret_cast< const char* >( sqlite3_column_text( pStmt, 2 ) ) );
    }

    if( !siteName.empty() && !userName.empty() && !userPass.empty() )
    {
        std::cout << "Site name: " << siteName << std::endl;
        std::cout << "User name: " << userName << std::endl;
        std::cout << "User password: " << userPass << std::endl;
    }
    else
    {
        std::cerr << "No such record in database." << std::endl;
    }
    sqlite3_finalize( pStmt );
    return EXIT_SUCCESS;
}

int ListDataBaseSites( sqlite3* db )
{
    char* errMsg;
    std::string sqlSelectReq( "SELECT SITE FROM PASSWORDS" );
    sqlite3_stmt *pStmt = NULL;
    if( SQLITE_OK != sqlite3_exec( db, sqlSelectReq.c_str(), callback, 0, &errMsg ) )
    {
        std::cerr << "Problems with req exec" << std::endl
                  << "SQL error: " << errMsg << std::endl;
        sqlite3_free( errMsg );
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

int GetUserPassword( std::string& userPasswd )
{
    userPasswd = getpass( "" );
    return EXIT_SUCCESS;
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

void PrintHelp( char* progName )
{
    std::cout << "Usage: " << progName <<  " (-g) (-r) (-c)" << std::endl
              << "\t-g  flag   Get password" << std::endl
              << "\t-r  flag   Register"    << std::endl
              << "\t-c  flag   Change Password" << std::endl
              << "\t-l  flag   List all registered sites" << std::endl;
}

int ParseCmd( int argc, char* argv[] )
{
    int opt = 0;
    if ( argc == 1 )
    {
        PrintHelp( argv[ 0 ] );
        return EXIT_FAILURE;
    }
    while( -1 != ( opt = getopt( argc, argv, "rgcl")) )
    {
        switch ( opt )
        {
        case 'r':
            action = REGISTER;
            return EXIT_SUCCESS;
        case 'g':
            action = GET_PASSWORD;
            return EXIT_SUCCESS;
        case 'c':
            action = CHANGE_PASSWORD;
            return EXIT_SUCCESS;
        case 'l':
            action = LIST_REGISTERED_SITES;
            return EXIT_SUCCESS;
        default:
            PrintHelp( argv[ 0 ] );
            return EXIT_FAILURE;
        }
    }
}

int callback( void *NotUsed, int argc, char **argv, char **azColName )
{
    for( int i = 0; i < argc; i++ )
    {
        printf( "%s = %s\n", azColName[ i ], argv[ i ] ? argv[ i ] : "NULL" );
    }
    return 0;
}