#ifndef CHAMELIB_CURVE_PARAMS_H
#define CHAMELIB_CURVE_PARAMS_H

#include <string>
#include <sstream>
#include <pbc/pbc.h>

class CurveParams
{
    public:
    static const std::string a_param;
    static const std::string a1_param;
    static const std::string e_param;
    static const std::string i_param;
    static const std::string a_param_80;
    static const std::string a_param_112;
    static const std::string a_param_128;
    static const std::string a_param_160;

    static const std::string sm9_param;
    static const std::string d159_param;
    static const std::string d201_param;
    static const std::string d224_param;
    static const std::string d105171_196_185_param;
    static const std::string d277699_175_167_param;
    static const std::string d278027_190_181_param;
    static const std::string f_param;
    static const std::string g149_param;
    
    static void GetMpz(mpz_t res, std::string s, int curve);
};

class Curve{
    public:
        const static int A = 0;
        const static int A1 = 1;
        const static int E = 2;
        const static int I = 3;
        const static int A_80 = 4;
        const static int A_112 = 5;
        const static int A_128 = 6;
        const static int A_160 = 7;
        const static int SM9 = 8;
        const static int D_159 = 9;
        const static int D_201 = 10;
        const static int D_224 = 11;
        const static int D_105171_196_185 = 12;
        const static int D_277699_175_167 = 13;
        const static int D_278027_190_181 = 14;
        const static int F = 15;
        const static int G_149 = 16;

        const static std::string curve_names[17];
        const static std::string group_names[4];
};

class Group{
    public:
        const static int G1 = 0;
        const static int G2 = 1;
        const static int GT = 2;
        const static int Zn = 3;
};

#endif  // CHAMELIB_CURVE_PARAMS_H
