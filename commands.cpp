#include <commands.h>

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
    // std::vector< uint8_t > userPasswd;
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
    // std::string userPasswdStr( reinterpret_cast< char* >( userPasswd.data()), userPasswd.size() );
    if( InsertToDataBase( passwordsBase, siteAddr, userName, userPasswd ) )
    {
        std::cerr << "Error adding to database." << std::endl;
    }
    sqlite3_close( passwordsBase );
}
