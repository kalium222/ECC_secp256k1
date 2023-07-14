#include <fstream>
#include <getopt.h>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ECC.h"

using std::cout, std::endl, std::string;
using sstream = std::stringstream;

// Store the cihpertext
static const string CHALLENGE_CIPHERTEXT = "Zlove;VEHFO";
static const string allchars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.;?!()";

enum Operation {
  ENCRYPT,
  DECRYPT,
  GENERATE,
  NONE
};

int main(int argc, char* argv[])
{
  // test();
  // return 0;

  int opt;
  string message, keyloc = "";
  Operation operation = NONE;
  string key = generate(true);

  static option opts[] = { { "generate", no_argument, NULL, 'g' },
    { "decrypt", required_argument, NULL, 'd' },
    { "encrypt", required_argument, NULL, 'e' },
    { "key", required_argument, NULL, 'k' },
    { NULL, 0, NULL, 0 } };
  while ((opt = getopt_long(argc, argv, ":gd:e:k:", opts, NULL)) != -1)
    switch (opt) {
    case 'g': {
      if (operation == NONE)
        operation = GENERATE;
      break;
    }
    case 'd': {
      if (operation == NONE)
      {
        operation = DECRYPT;
        message = optarg;
      }
      break;
    }
    case 'e': {
      if (operation == NONE)
      {
        operation = ENCRYPT;
        message = optarg;
      }
      break;
    }
    case 'k': {
      keyloc = optarg;
      break;
    }
    }

  if (keyloc != "") {
    std::fstream keyfile(keyloc);
    keyfile >> key;
    char c;
    keyfile.getline(&c, 1);
    if (keyfile.fail() || c != '\0')
      throw std::runtime_error("invalid key");
    keyfile.close();
  }
  switch (operation) {
  case GENERATE: {
    cout << generate() << endl;
    break;
  }
  case DECRYPT: {
    if (message == CHALLENGE_CIPHERTEXT && keyloc == "")
      cout << "cheater: it is forbidden to decrypt the challenge ciphertext" << endl;
    else
      cout << decrypt(message, key) << endl;
    break;
  }
  case ENCRYPT: {
    cout << encrypt(message, key) << endl;
    break;
  }
  case NONE: {
    cout << "there is nothing to do" << endl;
    break;
  }
  }
  return 0;
}
