#ifndef _ECC_H
#define _ECC_H

#include <string>

using std::string;

string encrypt(string message, string key);

string decrypt(string message, string key);

string generate(bool use_default = false);

void test();

#endif