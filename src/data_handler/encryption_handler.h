#ifndef ENCRYPTION_HANDLER
#define ENCRYPTION_HANDLER

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <fstream>
#include <stdexcept>

static const int AES_KEY_SIZE = 256;  // 256-bit key
static const int AES_BLOCK_SIZE = 16; // AES block size (128 bits)

class EncryptionHandler {
public:

	EncryptionHandler();

	~EncryptionHandler();

	bool CheckKeyAndIVExist();

	void SaveKeyAndIV();

	void LoadKeyAndIV();

	void HandleErrors();

	std::string Base64Encode(const unsigned char *data, int length);

	std::vector<unsigned char> Base64Decode(const std::string &base64);

	std::string EncryptMessage(const std::string plainText);

	std::string DecryptMessage(const std::string cipherTextStr);

private:
	std::vector<unsigned char> key;
	std::vector<unsigned char> iv;
	std::string filename;
};

#endif