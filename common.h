#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <unistd.h>

inline int GetUserPassword( std::string& userPasswd )
{
    userPasswd = getpass( "" );
    return EXIT_SUCCESS;
}

#endif