#include <database_handler.h>

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
    if( SQLITE_OK != sqlite3_exec( db, sqlSelectReq.c_str(), callback, 0, &errMsg ) )
    {
        std::cerr << "Problems with req exec" << std::endl
                  << "SQL error: " << errMsg << std::endl;
        sqlite3_free( errMsg );
        return EXIT_SUCCESS;
    }

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

int callback( void *NotUsed, int argc, char **argv, char **azColName )
{
    for( int i = 0; i < argc; i++ )
    {
        printf( "%s = %s\n", azColName[ i ], argv[ i ] ? argv[ i ] : "NULL" );
    }
    return 0;
}