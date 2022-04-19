#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <unistd.h>
#include <crypt/cryptlib.h>
#include <crypt/pwdbased.h>
#include <crypt/sha.h>
#include <crypt/files.h>
#include <crypt/aes.h>
#include <crypt/modes.h>
#include <crypt/hex.h>
#include <crypt/config_int.h>

inline std::vector< uint8_t > EncryptPassw( std::string& passwd )
{
    std::vector< uint8_t > passwdHex;
    for( auto it : passwd )
    {
        passwdHex.push_back( static_cast< uint8_t >( it ) );
    }

    const char info1[] = "PKCS5_PBKDF2_HMAC key derivation";
    size_t ilen1 = strlen( info1 );

    const char info2[] = "PKCS5_PBKDF2_HMAC iv derivation";
    size_t ilen2 = strlen( info2 );

    std::vector< uint8_t > key;
    key.resize( CryptoPP::AES::DEFAULT_KEYLENGTH );
    std::vector< uint8_t > iv;
    iv.resize( CryptoPP::AES::BLOCKSIZE );

    CryptoPP::PKCS5_PBKDF2_HMAC< CryptoPP::SHA256 > pbkdf;

    pbkdf.DeriveKey( key.data(), key.size(), 0, passwdHex.data(), passwdHex.size(), info1, ilen1, 1024, 0.0f);
    pbkdf.DeriveKey( iv.data(), iv.size(), 0, passwdHex.data(), passwdHex.size(), info2, ilen2, 1024, 0.0f);

    return passwdHex;
}

inline int GetUserPassword( std::vector< uint8_t >& userPasswd )
{
    std::string userPasswdStr = getpass( "" );
    userPasswd = EncryptPassw( userPasswdStr );
    return EXIT_SUCCESS;
}


#endif