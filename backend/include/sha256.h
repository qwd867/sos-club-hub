#ifndef SHA256_H
#define SHA256_H

#include <string>

class Sha256 {
public:
    static std::string hash(const std::string& input);
};

#endif
