#include <iostream>
#include <string>
#include <cstdint>
#include <getopt.h>
#include <stdio.h>

#include <commands.h>

#define GET_PASSWORD          0
#define REGISTER              1
#define CHANGE_PASSWORD       3
#define LIST_REGISTERED_SITES 4

static int32_t action = REGISTER;

static void PrintHelp( char* progName );
static int ParseCmd( int argc, char* argv[] );

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
    return EXIT_SUCCESS;
}