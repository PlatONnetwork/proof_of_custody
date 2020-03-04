#pragma once

#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#include <string>

void Init_SSL_CTX(SSL_CTX*& ctx, const std::string& playerCRT, const std::string& rootCRT);
void Destroy_SSL_CTX(SSL_CTX* ctx);
void ShowCerts(SSL* ssl, const std::string CommonName, int verbose);
