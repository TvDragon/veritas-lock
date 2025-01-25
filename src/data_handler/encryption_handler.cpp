#include "encryption_handler.h"

EncryptionHandler::EncryptionHandler()
	:filename("key_iv.bin")
{
    // Set the sizes for the key and IV before using them
    key.resize(AES_KEY_SIZE);
    iv.resize(AES_BLOCK_SIZE);

	if (!RAND_bytes(key.data(), key.size()) || !RAND_bytes(iv.data(), iv.size())) {
        std::cerr << "Error generating random key/IV." << std::endl;
    }

	bool keyAndIVExists = CheckKeyAndIVExist();

	if (!keyAndIVExists) {
		SaveKeyAndIV();
	}

	LoadKeyAndIV();
}

EncryptionHandler::~EncryptionHandler() {
	key.clear();
	iv.clear();
}

bool EncryptionHandler::CheckKeyAndIVExist() {
	try {
        // Get the current working directory
        std::filesystem::path currentPath = std::filesystem::current_path();

        // Construct the file path
        std::filesystem::path filePath = currentPath / filename;

        // Check if the file exists
        if (std::filesystem::exists(filePath)) {
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
	return false;
}

void EncryptionHandler::SaveKeyAndIV() {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    outFile.write(reinterpret_cast<const char*>(key.data()), key.size());
    outFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
    outFile.close();
    std::cout << "Key and IV saved to file: " << filename << std::endl;
}

void EncryptionHandler::LoadKeyAndIV() {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    inFile.read(reinterpret_cast<char*>(key.data()), key.size());
    inFile.read(reinterpret_cast<char*>(iv.data()), iv.size());
    inFile.close();
    std::cout << "Key and IV loaded from file: " << filename << std::endl;
}

void EncryptionHandler::HandleErrors() {
    std::cerr << "An error occurred." << std::endl;
    exit(EXIT_FAILURE);
}

std::string EncryptionHandler::Base64Encode(const unsigned char *data, int length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Do not use newlines
    bio = BIO_push(b64, bio);

    BIO_write(bio, data, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
}

std::vector<unsigned char> EncryptionHandler::Base64Decode(const std::string &base64) {
    BIO *bio, *b64;
    int decodeLen = base64.length();
    std::vector<unsigned char> buffer(decodeLen);

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(base64.data(), base64.length());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Do not use newlines
    bio = BIO_push(b64, bio);

    int len = BIO_read(bio, buffer.data(), decodeLen);
    buffer.resize(len);
    BIO_free_all(bio);

    return buffer;
}

std::string EncryptionHandler::EncryptMessage(const std::string plainText) {
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) HandleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
        HandleErrors();

    std::vector<unsigned char> ciphertext(plainText.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    int len;
    int ciphertextLen;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                                reinterpret_cast<const unsigned char *>(plainText.data()), plainText.size()))
        HandleErrors();
    ciphertextLen = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) HandleErrors();
    ciphertextLen += len;

    ciphertext.resize(ciphertextLen);
    EVP_CIPHER_CTX_free(ctx);

    return Base64Encode(ciphertext.data(), ciphertext.size());
}

std::string EncryptionHandler::DecryptMessage(const std::string cipherTextStr) {
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) HandleErrors();

    std::vector<unsigned char> ciphertext = Base64Decode(cipherTextStr);

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()))
        HandleErrors();

    std::vector<unsigned char> plaintext(ciphertext.size());

    int len;
    int plaintextLen;

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()))
        HandleErrors();
    plaintextLen = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) HandleErrors();
    plaintextLen += len;

    plaintext.resize(plaintextLen);
    EVP_CIPHER_CTX_free(ctx);

    return std::string(plaintext.begin(), plaintext.end());	
}