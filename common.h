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

inline std::string EncryptPasswd( std::string& passwd, std::string& userMasterPasswdStr )
{
    const char info1[] = "PKCS5_PBKDF2_HMAC key derivation";
    size_t ilen1 = strlen( info1 );

    const char info2[] = "PKCS5_PBKDF2_HMAC iv derivation";
    size_t ilen2 = strlen( info2 );

    std::vector< uint8_t > key;
    key.resize( CryptoPP::AES::DEFAULT_KEYLENGTH );
    std::vector< uint8_t > iv;
    iv.resize( CryptoPP::AES::BLOCKSIZE );

    CryptoPP::PKCS5_PBKDF2_HMAC< CryptoPP::SHA256 > pbkdf;

    pbkdf.DeriveKey( key.data(), key.size(), 0, reinterpret_cast< unsigned char* >( const_cast< char* >( passwd.data() ) ),
                     passwd.size(), reinterpret_cast< const unsigned char* >( info1 ), ilen1, 1024, 0.0f );
    pbkdf.DeriveKey( iv.data(), iv.size(), 0, reinterpret_cast< unsigned char* >( const_cast< char* >( passwd.data() ) ),
                     passwd.size(), reinterpret_cast< const unsigned char* >( info2 ), ilen2, 1024, 0.0f );

    CryptoPP::CBC_Mode< CryptoPP::AES>::Encryption encryption;
    encryption.SetKeyWithIV( key.data(), key.size(), iv.data(), iv.size() );
    CryptoPP::StreamTransformationFilter encryptor( encryption, NULL);

    for( auto passwdIt : passwd )
    {
        encryptor.Put( passwdIt );
    }

    encryptor.MessageEnd();

    size_t ready = encryptor.MaxRetrievable();

    std::string encodedPasswd( ready, 0x00 );
    encryptor.Get( reinterpret_cast< uint8_t* >( &encodedPasswd[0] ), encodedPasswd.size() );

    return encodedPasswd;
}

inline int GetUserPassword( std::string& userPasswd )
{
    std::string userPasswdStr = getpass( "" );
    std::cout << "Enter master password: ";
    std::string userMasterPasswdStr = getpass( "" );
    std::cout << std::endl;
    if( userMasterPasswdStr.size() > 16 )
    {
        std::cout << "Please use master password no longer than 16 symbols and more than 0" << std::endl
                  << "Inputed pasword size was made 16 symbols." << std::endl;
        userMasterPasswdStr.resize( 16 );
    }
    userPasswd = EncryptPasswd( userPasswdStr, userMasterPasswdStr );
    return EXIT_SUCCESS;
}


#endif