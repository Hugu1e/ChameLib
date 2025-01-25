#ifndef CHAMELIB_COMMONTEST_H
#define CHAMELIB_COMMONTEST_H

#include <pbc.h>
#include <gmp.h>
#include <curve/params.h>
#include <chrono>
#include <iostream>
#include <string>
#include <utils/Logger.h>

class CommonTest{
    private :
        bool out_file;
        bool visiable;

        std::chrono::_V2::system_clock::time_point ts, te;

        std::string current_test_name;

        FILE *out = NULL;
        
        CurveParams curves;
        pbc_param_t par;
        pairing_t pairing;

        element_t G1, G2, GT, Zn;
        
    
    public:
        CommonTest(std::string test_name, std::string curve, bool out_file = true, bool visiable = false);
        ~CommonTest();

        void init_type(std::string param);

        void OutTime(std::string name, int id, double us);

        void start(std::string current_test_name);
        void end(std::string test_name);

        element_s *get_G1() { return G1; }
        element_s *get_G2() { return G2; }
        element_s *get_GT() { return GT; }
        element_s *get_Zn() { return Zn; }
};

#endif  // CHAMELIB_COMMONTEST_H