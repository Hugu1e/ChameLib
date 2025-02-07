#include <AE/PKE_CCA_AMV_2017.h>

PKE_CCA_AMV_2017::PKE_CCA_AMV_2017(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn):PbcScheme(_G1, _G2, _GT, _Zn){}

void PKE_CCA_AMV_2017::KeyGen(PKE_CCA_AMV_2017_pk *pk, PKE_CCA_AMV_2017_sk *sk){
    element_random(tmp_G);
    element_random(tmp_G_2);
    pk->insertElement("g1","G1",tmp_G);
    pk->insertElement("g2","G1",tmp_G_2);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk->insertElement("x11", "Zn", tmp_Zn);
    sk->insertElement("x21", "Zn", tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk->insertElement("y1","G1",tmp_G_3);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk->insertElement("x12", "Zn", tmp_Zn);
    sk->insertElement("x22", "Zn", tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk->insertElement("y2","G1",tmp_G_3);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk->insertElement("x13", "Zn", tmp_Zn);
    sk->insertElement("x23", "Zn", tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk->insertElement("y3","G1",tmp_G_3);
}

void PKE_CCA_AMV_2017::Encrypt(PKE_CCA_AMV_2017_c *c, element_t m, PKE_CCA_AMV_2017_pk *pk){
    element_random(tmp_Zn);
    c->insertElement("rho", "Zn", tmp_Zn);

    // c1 = g1^rho
    element_pow_zn(tmp_G, pk->getElement("g1"), tmp_Zn);
    c->insertElement("c1","G1",tmp_G);

    // c2 = g2^rho
    element_pow_zn(tmp_G_2, pk->getElement("g2"), tmp_Zn);
    c->insertElement("c2","G1",tmp_G_2);

    // c3 = y^rho * H(m)
    element_pow_zn(tmp_G_3, pk->getElement("y3"), tmp_Zn);
    TypeConverter::element_g_from_element_zn(tmp_G_4, m);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    c->insertElement("c3","G1",tmp_G_3);
    // TODO use c3' = y^rho * m to avoid type convertion of m
    element_pow_zn(tmp_G_4, pk->getElement("y3"), tmp_Zn);
    element_mul(tmp_G_4, tmp_G_4, m);
    c->insertElement("c3_","G1",tmp_G_4);


    // c4 = y1^rho * y2^rho*t
    HASH::hash(tmp_Zn_2, tmp_G, tmp_G_2, tmp_G_3);
    element_pow_zn(tmp_G, pk->getElement("y1"), tmp_Zn);
    element_pow_zn(tmp_G_2, pk->getElement("y2"), tmp_Zn);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    c->insertElement("c4","G1",tmp_G);
}

void PKE_CCA_AMV_2017::Decrypt(element_t m, PKE_CCA_AMV_2017_c *c, PKE_CCA_AMV_2017_sk *sk){
    // m = c3/(c1^x1 * c2^x2)
    element_pow_zn(tmp_G, c->getElement("c1"), sk->getElement("x13"));
    element_pow_zn(tmp_G_2, c->getElement("c2"), sk->getElement("x23"));
    element_mul(tmp_G, tmp_G, tmp_G_2);
    element_div(tmp_G_3, c->getElement("c3"), tmp_G);
    TypeConverter::element_zn_from_element_g(m, tmp_G_3);
}

PKE_CCA_AMV_2017::~PKE_CCA_AMV_2017(){}
