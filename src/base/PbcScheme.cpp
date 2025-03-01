#include "base/PbcScheme.h"

PbcScheme::PbcScheme(const int curve){
    initCurve(curve);
}

void PbcScheme::initCurve(const int curve){
    CurveParams curves;

    switch(curve){
        case Curve::A:
            initPairing(curves.a_param);
            break;
        case Curve::A1:
            initPairing(curves.a1_param);
            break;
        case Curve::E:
            initPairing(curves.e_param);
            break;
        case Curve::I:
            initPairing(curves.i_param);
            break;
        case Curve::A_80:
            initPairing(curves.a_param_80);
            break;
        case Curve::A_112:
            initPairing(curves.a_param_112);
            break;
        case Curve::A_128:
            initPairing(curves.a_param_128);
            break;
        case Curve::A_160:
            initPairing(curves.a_param_160);
            break;
        case Curve::SM9:
            initPairing(curves.sm9_param);
            break;
        case Curve::D_159:
            initPairing(curves.d159_param);
            break;
        case Curve::D_201:
            initPairing(curves.d201_param);
            break;
        case Curve::D_224:
            initPairing(curves.d224_param);
            break;
        case Curve::D_105171_196_185:
            initPairing(curves.d105171_196_185_param);
            break;
        case Curve::D_277699_175_167:
            initPairing(curves.d277699_175_167_param);
            break;
        case Curve::D_278027_190_181:
            initPairing(curves.d278027_190_181_param);
            break;
        case Curve::F:
            initPairing(curves.f_param);
            break;
        case Curve::G_149:
            initPairing(curves.g149_param);
            break;
        default:
            throw CurveException(CurveException::INVALID_CURVE);
    }
}

void PbcScheme::initPairing(std::string param) {
    pbc_param_t par;
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);
}

void PbcScheme::init(element_t _G1, element_t _G2, element_t _GT, element_t _Zn){
    element_init_same_as(G1, _G1);
    element_init_same_as(G2, _G2);
    element_init_same_as(GT, _GT);
    element_init_same_as(Zn, _Zn);
}

void PbcScheme::init(element_t _G, element_t _Zn){
    element_init_same_as(G1, _G);
    element_init_same_as(Zn, _Zn);
}

element_s* PbcScheme::GetZrElement(){
    element_s *zr = new element_s;
    element_init_same_as(zr, this->Zn);
    element_random(zr);
    return zr;
}

element_s* PbcScheme::GetG1Element(){
    element_s *g1 = new element_s;
    element_init_same_as(g1, this->G1);
    element_random(g1);
    return g1;
}

element_s* PbcScheme::GetG2Element(){
    element_s *g2 = new element_s;
    element_init_same_as(g2, this->G2);
    element_random(g2);
    return g2;
}

element_s* PbcScheme::GetGTElement(){
    element_s *gt = new element_s;
    element_init_same_as(gt, this->GT);
    element_random(gt);
    return gt;
}