#ifndef misp_h
#define misp_h

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include "execution.h"
#include "strategies.h"

int main(int argc, char** argv);

std::vector<std::string> params_to_vector(int argc, char** argv);
void process_flags(std::vector<std::string> flags);

template<typename T>
bool vector_contains(std::vector<T> &haystack, T needle) {
  return !!(std::find(haystack.begin(), haystack.end(), needle) != haystack.end());
}

bool svector_contains(std::vector<std::string> &haystack, std::string needle) {
  return vector_contains<std::string>(haystack, needle);
}

#endif


