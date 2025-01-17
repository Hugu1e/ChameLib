#ifndef CHAMELIB_HASH_H
#define CHAMELIB_HASH_H

#include <openssl/sha.h>
#include <pbc.h>
#include <string>

class HASH{
    public:
        static void hash(element_t res, element_t m);
        static void hash(element_t res, std::string m);
};

#endif  // CHAMELIB_HASH_H