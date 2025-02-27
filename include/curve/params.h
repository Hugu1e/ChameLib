#ifndef CHAMELIB_CURVE_PARAMS_H
#define CHAMELIB_CURVE_PARAMS_H

#include <string>

class CurveParams
{
    public:
    std::string a_param;
    std::string a1_param;
    std::string e_param;
    std::string i_param;
    std::string a_param_80;
    std::string a_param_112;
    std::string a_param_128;
    std::string a_param_160;

    std::string sm9_param;
    std::string d159_param;
    std::string d201_param;
    std::string d224_param;
    std::string d105171_196_185_param;
    std::string d277699_175_167_param;
    std::string d278027_190_181_param;
    std::string f_param;
    std::string g149_param;
    
    CurveParams();
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
};

class Group{
    public:
        const static int G1 = 0;
        const static int G2 = 1;
        const static int GT = 2;
        const static int Zn = 3;
};

#endif  // CHAMELIB_CURVE_PARAMS_H
