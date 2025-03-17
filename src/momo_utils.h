#ifndef MOMO_UTILS_H
#define MOMO_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>


// AES-256 Key Size
#define AES_KEY_SIZE 32
#define AES_IV_SIZE 12
#define AES_TAG_SIZE 16

// Function to convert a HEX string to a vector of bytes
std::vector<unsigned char> HexToBytes(const std::string& hex);

// Function to check if a string ends with a specific suffix
bool EndsWith(const std::string& str, const std::string& suffix);

// Function to load the encrypted model from a file
bool LoadEncryptedModel(const std::string& filename, std::vector<unsigned char>& iv,
                        std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& tag);

// Function to decrypt model data
bool DecryptModelData(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                      const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag,
                      std::string& decrypted_text);

// Function to decrypt a TorchScript model
bool DecryptTorchScriptModel(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                             const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag);

// Function to decrypt a model from a file
bool DecryptModel(const std::string& filepath, const std::string& key, std::string* decrypted_model_data);

#endif // MOMO_UTILS_H