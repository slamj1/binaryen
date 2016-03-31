/*
 * Copyright 2015 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "support/file.h"

#include <cstdlib>
#include <limits>

template <typename T>
T wasm::read_file(const std::string &filename, bool debug) {
  if (debug) std::cerr << "Loading '" << filename << "'..." << std::endl;
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    std::cerr << "Failed opening '" << filename << "'" << std::endl;
    exit(EXIT_FAILURE);
  }
  infile.seekg(0, std::ios::end);
  std::streampos insize = infile.tellg();
  if (uint64_t(insize) >= std::numeric_limits<size_t>::max()) {
    // Building a 32-bit executable where size_t == 32 bits, we are not able to create strings larger than 2^32 bytes in length, so must abort here.
    std::cerr << "Failed opening '" << filename << "': Input file too large: " << insize << " bytes. Try rebuilding in 64-bit mode." << std::endl;
    exit(EXIT_FAILURE);
  }
  T input(size_t(insize) + 1, '\0');
  infile.seekg(0);
  infile.read(&input[0], insize);
  return input;
}

// Explicit instantiations for the explicit specializations.
template std::string wasm::read_file<>(const std::string &, bool);
template std::vector<char> wasm::read_file<>(const std::string &, bool);

wasm::Output::Output(const std::string &filename, bool debug)
    : outfile(), out([this, filename, debug]() {
        std::streambuf *buffer;
        if (filename.size()) {
          if (debug) std::cerr << "Opening '" << filename << std::endl;
          outfile.open(filename, std::ofstream::out | std::ofstream::trunc);
          if (!outfile.is_open()) {
            std::cerr << "Failed opening '" << filename << "'" << std::endl;
            exit(EXIT_FAILURE);
          }
          buffer = outfile.rdbuf();
        } else {
          buffer = std::cout.rdbuf();
        }
        return buffer;
      }()) {}
