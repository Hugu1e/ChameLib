/*
 * @Author: hyj2862588507@163.com
 * @Date: 2025-03-09 13:46:28
 * @LastEditors: hyj2862588507@163.com
 * @LastEditTime: 2025-03-09 13:46:36
 * @FilePath: \ABET\RandomGenerator.h
 * @Description: 
 * 
 * Copyright (c) 2025 by hyj2862588507@163.com, All Rights Reserved. 
 */
#ifndef CHAMELIB_UTILS_RANDOMGENERATOR_H
#define CHAMELIB_UTILS_RANDOMGENERATOR_H

#include <pbc/pbc.h>
#include <sys/time.h>
#include <random>

class RandomGenerator{
    public:
        static void RandomInLength(mpz_t res, short length);
        static void RandomPrimeInLength(mpz_t res, short length);
        static void RandomN(mpz_t res, mpz_t n);
        static void RandomCoprimeN(mpz_t res, mpz_t n);
        static void Random_bytes(unsigned char* buffer, size_t size);
};

#endif  // CHAMELIB_UTILS_RANDOMGENERATOR_H