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
static const string CHALLENGE_CIPHERTEXT = "4001ede1824ef499e821883ca7fb0ba99cf869e4cf45456f2ef15e9f70dc558,8ac25b89da98b6400bdfc014b4e69e3750ec82a5e7ed574ff1629b4c68d80581,69cc065156af31fac1d43b8e15db38c4c59222396a1aa9b0e0551da0d1626ec,ff781ffdf1eeae34653fcafc691f22573bf5cd5042529028146010d21d9b8595";
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
