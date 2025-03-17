#include "momo_utils.h"


// Chuyển chuỗi HEX thành vector bytes
std::vector<unsigned char> HexToBytes(const std::string& hex) {
  std::vector<unsigned char> bytes;
  for (size_t i = 0; i < hex.length(); i += 2) {
      std::string byteString = hex.substr(i, 2);
      unsigned char byte = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
      bytes.push_back(byte);
  }
  return bytes;
}


// Function to check if a string ends with a specific suffix
bool EndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  }
  
  // Read the content of a text file into a string
bool LoadEncryptedModel(const std::string& filename, std::vector<unsigned char>& iv,
                        std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& tag) {
  
  std::ifstream file(filename, std::ios::binary);
  if (!file) return false;

  file.seekg(0, std::ios::end);
  size_t file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  iv.resize(AES_IV_SIZE);
  tag.resize(AES_TAG_SIZE);
  ciphertext.resize(file_size - AES_IV_SIZE - AES_TAG_SIZE);

  file.read((char*)iv.data(), AES_IV_SIZE);
  file.read((char*)ciphertext.data(), ciphertext.size());
  file.read((char*)tag.data(), AES_TAG_SIZE);

  file.close();
  return true;
}

bool DecryptModelData(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                      const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag,
                      std::string& decrypted_text) {
  
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return false;

  decrypted_text.resize(ciphertext.size());

  if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) return false;
  if (!EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data())) return false;

  int len;
  if (!EVP_DecryptUpdate(ctx, (unsigned char*)decrypted_text.data(), &len, ciphertext.data(), ciphertext.size())) return false;

  if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AES_TAG_SIZE, (void*)tag.data())) return false;

  if (!EVP_DecryptFinal_ex(ctx, (unsigned char*)decrypted_text.data() + len, &len)) return false;

  EVP_CIPHER_CTX_free(ctx);
  return true;
}


// Hàm giải mã TorchScript Model
bool DecryptTorchScriptModel(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                            const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag) {
  std::string decrypted_text;
  if (!DecryptModelData(ciphertext, key, iv, tag, decrypted_text)) {
  return false;
  }
  // std::istringstream buffer(decrypted_text);
  // model = torch::jit::load(buffer);
  return true;
}


bool DecryptModel(const std::string& filepath, const std::string& key, std::string* decrypted_model_data) {
  std::vector<unsigned char> loaded_key = HexToBytes(key);
  std::vector<unsigned char> loaded_iv;
  std::vector<unsigned char> loaded_tag;
  std::vector<unsigned char> loaded_ciphertext;

  if (!LoadEncryptedModel(filepath, loaded_iv, loaded_ciphertext, loaded_tag)) {
    return false;
  }

  return DecryptModelData(loaded_ciphertext, loaded_key, loaded_iv, loaded_tag, *decrypted_model_data);
}


