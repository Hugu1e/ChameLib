#include <AE/PKE_CCA_AMV_2017.h>

PKE_CCA_AMV_2017::PKE_CCA_AMV_2017(int curve, int group):PbcScheme(curve){
    switch(group){
        case Group::G1:
            element_init_G1(G1, pairing);
            break;
        case Group::G2:
            element_init_G2(G1, pairing);
            break;
        case Group::GT:
            element_init_GT(G1, pairing);
            break;
        default:
            throw CurveException(CurveException::INVALID_GROUP);
    }
    element_init_Zr(Zn, pairing);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_G_4, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

void PKE_CCA_AMV_2017::init(element_t _G, element_t _Zn){
    element_init_same_as(G1, _G);
    element_init_same_as(Zn, _Zn);

    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_G_4, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
}

void PKE_CCA_AMV_2017::Setup(PKE_CCA_AMV_2017_pk &pk, PKE_CCA_AMV_2017_sk &sk, PKE_CCA_AMV_2017_c &c){
    pk.init(5);
    sk.init(6);
    c.init(6);
}

void PKE_CCA_AMV_2017::KeyGen(PKE_CCA_AMV_2017_pk &pk, PKE_CCA_AMV_2017_sk &sk){
    element_random(tmp_G);
    element_random(tmp_G_2);
    pk.set(g1, tmp_G);
    pk.set(g2, tmp_G_2);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk.set(x11, tmp_Zn);
    sk.set(x21, tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk.set(y1, tmp_G_3);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk.set(x12, tmp_Zn);
    sk.set(x22, tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk.set(y2, tmp_G_3);

    element_random(tmp_Zn);
    element_random(tmp_Zn_2);
    sk.set(x13, tmp_Zn);
    sk.set(x23, tmp_Zn_2);
    element_pow_zn(tmp_G_3, tmp_G, tmp_Zn);
    element_pow_zn(tmp_G_4, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    pk.set(y3, tmp_G_3);
}

void PKE_CCA_AMV_2017::Encrypt(PKE_CCA_AMV_2017_c &c, element_t m, PKE_CCA_AMV_2017_pk &pk){
    element_random(tmp_Zn);
    c.set(rho, tmp_Zn);

    // c1 = g1^rho
    element_pow_zn(tmp_G, pk[g1], tmp_Zn);
    c.set(c1, tmp_G);

    // c2 = g2^rho
    element_pow_zn(tmp_G_2, pk[g2], tmp_Zn);
    c.set(c2, tmp_G_2);

    // c3 = y^rho * H(m)
    element_pow_zn(tmp_G_3, pk[y3], tmp_Zn);
    TypeConverter::element_g_from_element_zn(tmp_G_4, m);
    element_mul(tmp_G_3, tmp_G_3, tmp_G_4);
    c.set(c3, tmp_G_3);
    // TODO use c3' = y^rho * m to avoid type convertion of m
    element_pow_zn(tmp_G_4, pk[y3], tmp_Zn);
    element_mul(tmp_G_4, tmp_G_4, m);
    c.set(c3_, tmp_G_4);

    // c4 = y1^rho * y2^rho*t
    HASH::hash(tmp_Zn_2, tmp_G, tmp_G_2, tmp_G_3);
    element_pow_zn(tmp_G, pk[y1], tmp_Zn);
    element_pow_zn(tmp_G_2, pk[y2], tmp_Zn);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    c.set(c4, tmp_G);
}

void PKE_CCA_AMV_2017::Decrypt(element_t m, PKE_CCA_AMV_2017_c &c, PKE_CCA_AMV_2017_sk &sk){
    // t =H(c1, c2, c3)
    HASH::hash(tmp_Zn, c[c1], c[c2], c[c3]);
    // verify c4 = c1^(x11+x12*t) * c2^(x21+x22*t)
    element_mul(tmp_Zn_2, sk[x12], tmp_Zn);
    element_add(tmp_Zn_2, sk[x11], tmp_Zn_2);
    element_pow_zn(tmp_G, c[c1], tmp_Zn_2);
    element_mul(tmp_Zn_2, sk[x22], tmp_Zn);
    element_add(tmp_Zn_2, sk[x21], tmp_Zn_2);
    element_pow_zn(tmp_G_2, c[c2], tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    if(element_cmp(tmp_G, c[c4]) != 0){
        throw std::runtime_error("PKE_CCA_AMV_2017::Decrypt(): verify failed");
    }

    // m = c3/(c1^x1 * c2^x2)
    element_pow_zn(tmp_G, c[c1], sk[x13]);
    element_pow_zn(tmp_G_2, c[c2], sk[x23]);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    element_div(tmp_G_3, c[c3], tmp_G);
    TypeConverter::element_zn_from_element_g(m, tmp_G_3);
}

PKE_CCA_AMV_2017::~PKE_CCA_AMV_2017(){
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_G_4);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);

    element_clear(G1);
    element_clear(Zn);
}
