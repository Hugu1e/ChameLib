#include <AE/PKE_CPA_AMV_2017.h>

PKE_CPA_AMV_2017::PKE_CPA_AMV_2017(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn){}

void PKE_CPA_AMV_2017::Setup(PKE_CPA_AMV_2017_pk &pk, PKE_CPA_AMV_2017_sk &sk, PKE_CPA_AMV_2017_c &c){
    pk.init(2);
    sk.init(1);
    c.init(4);
}

void PKE_CPA_AMV_2017::KeyGen(PKE_CPA_AMV_2017_pk &pk, PKE_CPA_AMV_2017_sk &sk){
    element_random(tmp_G);

    // x
    element_random(tmp_Zn);
    sk.set(x, tmp_Zn);

    // pk = (g, g^x)
    element_pow_zn(tmp_G_2, tmp_G, tmp_Zn);
    pk.set(g, tmp_G);
    pk.set(y, tmp_G_2);
}

void PKE_CPA_AMV_2017::Encrypt(PKE_CPA_AMV_2017_c &c, element_t m, PKE_CPA_AMV_2017_pk &pk){
    element_random(tmp_Zn);
    c.set(rho, tmp_Zn);

    // c1 = g^rho
    element_pow_zn(tmp_G, pk[g], tmp_Zn);
    c.set(c1, tmp_G);

    // c2 = y^rho * H(m)
    element_pow_zn(tmp_G, pk[y], tmp_Zn);
    TypeConverter::element_g_from_element_zn(tmp_G_2, m);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    c.set(c2, tmp_G);
    // TODO use c2' = y^rho * m to avoid type convertion of m
    // c2' = y^rho * m
    element_pow_zn(tmp_G_3, pk[y], tmp_Zn);
    element_mul(tmp_G_3, tmp_G_3, m);
    c.set(c2_, tmp_G_3);
}

void PKE_CPA_AMV_2017::Decrypt(element_t m, PKE_CPA_AMV_2017_c &c, PKE_CPA_AMV_2017_sk &sk){
    // m = c2/c1^x
    element_pow_zn(tmp_G, c[c1], sk[x]);
    element_div(tmp_G_2, c[c2], tmp_G);
    TypeConverter::element_zn_from_element_g(m, tmp_G_2);
}

PKE_CPA_AMV_2017::~PKE_CPA_AMV_2017(){}
