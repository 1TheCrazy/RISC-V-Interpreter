using namespace std;

#include <fstream>
#include <vector>
#include "util/FileUtil.h"

// Util for reading a file in bytes
vector<uint8_t> read_file_bytes(const string& path) {
    ifstream f(path, ios::binary);
    if (!f) throw runtime_error("Failed to open file: " + path);

    f.seekg(0, ios::end);
    const streamsize size = f.tellg();
    if (size < 0) throw runtime_error("tellg failed");

    vector<uint8_t> data(static_cast<size_t>(size));

    f.seekg(0, ios::beg);
    if (!f.read(reinterpret_cast<char*>(data.data()), size))
        throw runtime_error("Failed to read file: " + path);

    return data;
}