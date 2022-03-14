#pragma once

json loadJson(const std::string& filename);
void dbg(const char* format, ...);
bool fatal(const char* format, ...);
bool isPressed(int key);

uint32_t getID(const void* buff, size_t nbytes);
uint32_t getID(const char* txt);

bool fileExists( const char* name );

#include "data_provider.h"
#include "data_saver.h"
#include "utils/time.h"
#include "utils/file_context.h"
#include "utils/named_values.h"
#include "utils/buffer.h"

template <typename Out>
inline void split(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

inline bool isNumber(const std::string& s)
{
    return s.find_first_not_of("-.0123456789") == std::string::npos;
}

inline float unitRandom()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline float randomFloat(float vmin, float vmax)
{
    return vmin + (vmax - vmin) * unitRandom();
}
