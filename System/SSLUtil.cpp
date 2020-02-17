
#include <string>
#include <fstream>

#include "Exceptions/Exceptions.h"
#include "SSLUtil.h"

SSL_CTX* InitCTX(void) {
  const SSL_METHOD* method;
  SSL_CTX* ctx;

  method = TLS_method(); /* create new server-method instance */
  ctx = SSL_CTX_new(method); /* create new context from method */

  if (ctx == NULL) {
    ERR_print_errors_fp(stdout);
    throw SSL_error("InitCTX");
  }

  SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

  return ctx;
}

void LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile) {
  /* set the local certificate from CertFile */
  if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stdout);
    throw SSL_error("LoadCertificates 1");
  }
  /* set the private key from KeyFile (may be the same as CertFile) */
  if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stdout);
    throw SSL_error("LoadCertificates 2");
  }
  /* verify private key */
  if (!SSL_CTX_check_private_key(ctx)) {
    throw SSL_error("Private key does not match the public certificate");
  }
}

void ShowCerts(SSL* ssl, const string CommonName, int verbose) {
  X509* cert;
  char* line;

  cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
  if (cert != NULL) {
    if (verbose > 0) {
      printf("Server certificates:\n");
      line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
      printf("Subject: %s\n", line);
      free(line);
      line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
      printf("Issuer: %s\n", line);
      free(line);
    }

    char buffer[256];
    X509_NAME_get_text_by_NID(X509_get_subject_name(cert), NID_commonName, buffer, 256);
    string name(buffer);
    if (verbose > 0) {
      printf("Subject Comman Name:  %s\n", buffer);
    }
    if (name.compare(CommonName) != 0) {
      throw SSL_error("Common name does not match what I was expecting");
    }

    X509_free(cert);
  } else
    printf("No certificates.\n");
}

void Init_SSL_CTX(SSL_CTX*& ctx, const std::string& playerCRT, const std::string& rootCRT) {
  // Initialize the SSL library
  OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
  ctx = InitCTX();

  // Load in my certificates
  string str_crt = "Cert-Store/" + playerCRT;
  string str_key = str_crt.substr(0, str_crt.length() - 3) + "key";
  LoadCertificates(ctx, str_crt.c_str(), str_key.c_str());

  // Turn on client auth via cert
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

  // Load in root CA
  string str = "Cert-Store/" + rootCRT;
  SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(str.c_str()));
  SSL_CTX_load_verify_locations(ctx, str.c_str(), NULL);
}

void Destroy_SSL_CTX(SSL_CTX* ctx) {
  SSL_CTX_free(ctx);
}
