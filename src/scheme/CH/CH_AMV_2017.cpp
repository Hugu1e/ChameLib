#include "scheme/CH/CH_AMV_2017.h"

CH_AMV_2017::CH_AMV_2017(int curve, int group): PbcScheme(curve){
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

    PKE_CCA.init(G1, Zn);
    PKE_CPA.init(G1, Zn);
    
    element_init_same_as(tmp_G, G1);
    element_init_same_as(tmp_G_2, G1);
    element_init_same_as(tmp_G_3, G1);
    element_init_same_as(tmp_G_4, G1);
    element_init_same_as(tmp_Zn, Zn);
    element_init_same_as(tmp_Zn_2, Zn);
    element_init_same_as(tmp_Zn_3, Zn);
    element_init_same_as(tmp_Zn_4, Zn);
}

void CH_AMV_2017::SetUp(CH_AMV_2017_pk &pk, CH_AMV_2017_sk &sk, CH_AMV_2017_h &h, CH_AMV_2017_h &h_p){
    pk.get_CH_pk().init(2);
    sk.get_CH_sk().init(1);
    h.get_h().init(1);
    h.get_r().get_pai().init(13);
    h_p.get_h().init(1);
    h_p.get_r().get_pai().init(13);
    PKE_CPA.Setup(pk.get_PKE_CPA_pk(), sk.get_PKE_CPA_sk(), h.get_r().get_c1());
    PKE_CCA.Setup(pk.get_PKE_CCA_pk(), sk.get_PKE_CCA_sk(), h.get_r().get_c2());
    PKE_CPA.Setup(pk.get_PKE_CPA_pk(), sk.get_PKE_CPA_sk(), h_p.get_r().get_c1());
    PKE_CCA.Setup(pk.get_PKE_CCA_pk(), sk.get_PKE_CCA_sk(), h_p.get_r().get_c2());
}
void CH_AMV_2017::KeyGen(CH_AMV_2017_pk &pk, CH_AMV_2017_sk &sk){
    element_random(tmp_Zn);
    sk.get_CH_sk().set(x, tmp_Zn);

    // g
    element_random(tmp_G);
    pk.get_CH_pk().set(g, tmp_G);

    // y=g^x
    element_pow_zn(tmp_G, tmp_G, tmp_Zn);
    pk.get_CH_pk().set(y, tmp_G);

    PKE_CCA.KeyGen(pk.get_PKE_CCA_pk(), sk.get_PKE_CCA_sk());

    PKE_CPA.KeyGen(pk.get_PKE_CPA_pk(), sk.get_PKE_CPA_sk());    
}

void CH_AMV_2017::Hash(CH_AMV_2017_h &h, element_t m, CH_AMV_2017_pk &pk){
    // r
    element_random(tmp_Zn);

    // h=g^r y^-m
    element_pow_zn(tmp_G, pk.get_CH_pk()[g], tmp_Zn);
    element_pow_zn(tmp_G_2, pk.get_CH_pk()[y], m);
    element_div(tmp_G, tmp_G, tmp_G_2);
    h.get_h().set(h1, tmp_G);

    PKE_CPA.Encrypt(h.get_r().get_c1(), tmp_Zn, pk.get_PKE_CPA_pk());
    PKE_CCA.Encrypt(h.get_r().get_c2(), tmp_Zn, pk.get_PKE_CCA_pk());

    // pai
    // 1.pai_0
    // a
    element_random(tmp_Zn_2);
    // pai_0_1 = g^a
    element_pow_zn(tmp_G_2, pk.get_CH_pk()[g], tmp_Zn_2);
    h.get_r().get_pai().set(pai_0_1, tmp_G_2);
    // b
    element_pow_zn(tmp_G_3, pk.get_CH_pk()[y], m);
    element_mul(tmp_G_3, tmp_G, tmp_G_3);
    HASH::hash(tmp_Zn_3, tmp_G_3, tmp_G_2);
    // pai_0_2 = br + a
    element_mul(tmp_Zn, tmp_Zn_3, tmp_Zn);
    element_add(tmp_Zn, tmp_Zn, tmp_Zn_2);
    h.get_r().get_pai().set(pai_0_2, tmp_Zn);

    // 2.pai_1
    // a1
    element_random(tmp_Zn);
    // pai_1_1 = g^a1
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[g], tmp_Zn);
    h.get_r().get_pai().set(pai_1_1, tmp_G);
    // b1
    HASH::hash(tmp_Zn_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::c1], tmp_G);
    // pai_1_2 = b1rho1 + a1
    element_mul(tmp_Zn_2, tmp_Zn_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::rho]);
    element_add(tmp_Zn_2, tmp_Zn_2, tmp_Zn);
    h.get_r().get_pai().set(pai_1_2, tmp_Zn_2);

    // 3.pai_2
    // a2
    element_random(tmp_Zn);
    // pai_2_1 = g1^a2
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g1], tmp_Zn);
    h.get_r().get_pai().set(pai_2_1, tmp_G);
    // pai_2_2 = g2^a2
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g2], tmp_Zn);
    h.get_r().get_pai().set(pai_2_2, tmp_G_2);
    // pai_2_3 = b2rho2 + a2
    // b2
    HASH::hash(tmp_Zn_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h.get_r().get_c2()[PKE_CCA_AMV_2017::c2], tmp_G, tmp_G_2);
    element_mul(tmp_Zn_2, tmp_Zn_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_add(tmp_Zn_2, tmp_Zn_2, tmp_Zn);
    h.get_r().get_pai().set(pai_2_3, tmp_Zn_2);

    // 4.pai_3
    // a31
    element_random(tmp_Zn);
    // a32
    element_random(tmp_Zn_2);
    // pai_3_1 = y1^a31 * y2^a32
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1], tmp_Zn);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2], tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    h.get_r().get_pai().set(pai_3_1, tmp_G);
    // b3
    HASH::hash(tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::c4], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2], tmp_G);
    // pai_3_2 = b3rho2 + a31
    element_mul(tmp_Zn_4, tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn);
    h.get_r().get_pai().set(pai_3_2, tmp_Zn_4);
    // pai_3_3 = b3rho2t + a32
    element_mul(tmp_Zn_4, tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    HASH::hash(tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h.get_r().get_c2()[PKE_CCA_AMV_2017::c2], h.get_r().get_c2()[PKE_CCA_AMV_2017::c3]);
    element_mul(tmp_Zn_4, tmp_Zn_4, tmp_Zn_3);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn_2);
    h.get_r().get_pai().set(pai_3_3, tmp_Zn_4);

    // 5.pai_4
    // a41
    element_random(tmp_Zn);
    // a42
    element_random(tmp_Zn_2);
    // pai_4_1 = y^a41 y3^a42
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[y], tmp_Zn);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    h.get_r().get_pai().set(pai_4_1, tmp_G);
    // b4
    element_div(tmp_G_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::c2_], h.get_r().get_c2()[PKE_CCA_AMV_2017::c3_]);
    HASH::hash(tmp_Zn_3, tmp_G_2, pk.get_PKE_CPA_pk()[y], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], tmp_G);
    // y41 = b4rho1 + a41
    element_mul(tmp_Zn_4, tmp_Zn_3, h.get_r().get_c1()[PKE_CPA_AMV_2017::rho]);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn);
    h.get_r().get_pai().set(pai_4_2, tmp_Zn_4);
    // y42 = -b4rho2 + a42
    element_mul(tmp_Zn_4, tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_sub(tmp_Zn_4, tmp_Zn_2, tmp_Zn_4);
    h.get_r().get_pai().set(pai_4_3, tmp_Zn_4);
}

bool CH_AMV_2017::Check(CH_AMV_2017_h &h, element_t m, CH_AMV_2017_pk &pk){
    // 1.
    // g^y/a
    element_pow_zn(tmp_G, pk.get_CH_pk()[g], h.get_r().get_pai()[pai_0_2]);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_0_1]);
    // (hy^m)^b
    // b
    element_pow_zn(tmp_G_2, pk.get_CH_pk()[y], m);
    element_mul(tmp_G_2, h.get_h()[h1], tmp_G_2);
    HASH::hash(tmp_Zn, tmp_G_2, h.get_r().get_pai()[pai_0_1]);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }

    // 2.
    // g^y1/a1
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[g], h.get_r().get_pai()[pai_1_2]);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_1_1]);
    // c1^b1
    // b1
    HASH::hash(tmp_Zn_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::c1], h.get_r().get_pai()[pai_1_1]);
    element_pow_zn(tmp_G_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::c1], tmp_Zn_2);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }

    // 3.
    // g1^y2/a1
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g1], h.get_r().get_pai()[pai_2_3]);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_2_1]);
    // c1^b2
    HASH::hash(tmp_Zn_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h.get_r().get_c2()[PKE_CCA_AMV_2017::c2], h.get_r().get_pai()[pai_2_1], h.get_r().get_pai()[pai_2_2]);
    element_pow_zn(tmp_G_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c1], tmp_Zn_2);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }
    // g2^y2/a2
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g2], h.get_r().get_pai()[pai_2_3]);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_2_2]);
    // c1^b2
    HASH::hash(tmp_Zn_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h.get_r().get_c2()[PKE_CCA_AMV_2017::c2], h.get_r().get_pai()[pai_2_1], h.get_r().get_pai()[pai_2_2]);
    element_pow_zn(tmp_G_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c2], tmp_Zn_2);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }

    // 4.
    // y1^y31 y2^y32 / a3
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1], h.get_r().get_pai()[pai_3_2]);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2], h.get_r().get_pai()[pai_3_3]);
    element_mul(tmp_G, tmp_G,tmp_G_2);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_3_1]);
    // c24^b3
    // b3
    HASH::hash(tmp_Zn_3, h.get_r().get_c2()[PKE_CCA_AMV_2017::c4], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2], h.get_r().get_pai()[pai_3_1]);
    element_pow_zn(tmp_G_2, h.get_r().get_c2()[PKE_CCA_AMV_2017::c4], tmp_Zn_3);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }

    // 5.
    // y^y41 y3^y42 / a4
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[y], h.get_r().get_pai()[pai_4_2]);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], h.get_r().get_pai()[pai_4_3]);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    element_div(tmp_G, tmp_G, h.get_r().get_pai()[pai_4_1]);
    // c12/c23 ^b4
    // b4
    element_div(tmp_G_2, h.get_r().get_c1()[PKE_CPA_AMV_2017::c2_], h.get_r().get_c2()[PKE_CCA_AMV_2017::c3_]);
    HASH::hash(tmp_Zn_3, tmp_G_2, pk.get_PKE_CPA_pk()[y], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], h.get_r().get_pai()[pai_4_1]);
    element_pow_zn(tmp_G_2, tmp_G_2, tmp_Zn_3);
    // Logger::PrintPbc("tmp_G",tmp_G);
    // Logger::PrintPbc("tmp_G_2",tmp_G_2);
    if(element_cmp(tmp_G, tmp_G_2) != 0){
        return false;
    }

    return true;
}

void CH_AMV_2017::Adapt(CH_AMV_2017_h &h_p, element_t m_p, CH_AMV_2017_h &h, element_t m, CH_AMV_2017_sk &sk, CH_AMV_2017_pk &pk){
    // Check
    if(!Check(h, m, pk)){
        throw std::runtime_error("CH_AMV_2017::Adapt(): Hash Check failed.");
    }
    // r
    PKE_CPA.Decrypt(tmp_Zn, h.get_r().get_c1(), sk.get_PKE_CPA_sk());
    // r' = r - x(m-m')
    element_sub(tmp_Zn_2, m, m_p);
    element_mul(tmp_Zn_2, tmp_Zn_2, sk.get_CH_sk()[x]);
    element_sub(tmp_Zn, tmp_Zn, tmp_Zn_2);
    
    h_p.get_h().set(h1, h.get_h()[h1]);

    PKE_CPA.Encrypt(h_p.get_r().get_c1(), tmp_Zn, pk.get_PKE_CPA_pk());
    PKE_CCA.Encrypt(h_p.get_r().get_c2(), tmp_Zn, pk.get_PKE_CCA_pk());

    // pai
    // 1.pai_0
    // a
    element_random(tmp_Zn_2);
    // pai_0_1 = g^a
    element_pow_zn(tmp_G_2, pk.get_CH_pk()[g], tmp_Zn_2);
    h_p.get_r().get_pai().set(pai_0_1, tmp_G_2);
    // b
    element_pow_zn(tmp_G_3, pk.get_CH_pk()[y], m_p);
    element_mul(tmp_G_3, h_p.get_h()[h1], tmp_G_3);
    HASH::hash(tmp_Zn_3, tmp_G_3, tmp_G_2);
    // pai_0_2 = br + a
    element_mul(tmp_Zn, tmp_Zn_3, tmp_Zn);
    element_add(tmp_Zn, tmp_Zn, tmp_Zn_2);
    h_p.get_r().get_pai().set(pai_0_2, tmp_Zn);

    // 2.pai_1
    // a1
    element_random(tmp_Zn);
    // pai_1_1 = g^a1
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[g], tmp_Zn);
    h_p.get_r().get_pai().set(pai_1_1, tmp_G);
    // b1
    HASH::hash(tmp_Zn_2, h_p.get_r().get_c1()[PKE_CPA_AMV_2017::c1], tmp_G);
    // pai_1_2 = b1rho1 + a1
    element_mul(tmp_Zn_2, tmp_Zn_2, h_p.get_r().get_c1()[PKE_CPA_AMV_2017::rho]);
    element_add(tmp_Zn_2, tmp_Zn_2, tmp_Zn);
    h_p.get_r().get_pai().set(pai_1_2, tmp_Zn_2);

    // 3.pai_2
    // a2
    element_random(tmp_Zn);
    // pai_2_1 = g1^a2
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g1], tmp_Zn);
    h_p.get_r().get_pai().set(pai_2_1, tmp_G);
    // pai_2_2 = g2^a2
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::g2], tmp_Zn);
    h_p.get_r().get_pai().set(pai_2_2, tmp_G_2);
    // pai_2_3 = b2rho2 + a2
    // b2
    HASH::hash(tmp_Zn_2, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c2], tmp_G, tmp_G_2);
    element_mul(tmp_Zn_2, tmp_Zn_2, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_add(tmp_Zn_2, tmp_Zn_2, tmp_Zn);
    h_p.get_r().get_pai().set(pai_2_3, tmp_Zn_2);

    // 4.pai_3
    // a31
    element_random(tmp_Zn);
    // a32
    element_random(tmp_Zn_2);
    // pai_3_1 = y1^a31 * y2^a32
    element_pow_zn(tmp_G, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1],tmp_Zn);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2],tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    h_p.get_r().get_pai().set(pai_3_1, tmp_G);
    // b3
    HASH::hash(tmp_Zn_3, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c4], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y1], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y2],tmp_G);
    // pai_3_2 = b3rho2 + a31
    element_mul(tmp_Zn_4, tmp_Zn_3, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn);
    h_p.get_r().get_pai().set(pai_3_2,tmp_Zn_4);
    // pai_3_3 = b3rho2t + a32
    element_mul(tmp_Zn_4, tmp_Zn_3, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    HASH::hash(tmp_Zn_3, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c1], h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c2], h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c3]);
    element_mul(tmp_Zn_4, tmp_Zn_4, tmp_Zn_3);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn_2);
    h_p.get_r().get_pai().set(pai_3_3,tmp_Zn_4);

    // 5.pai_4
    // a41
    element_random(tmp_Zn);
    // a42
    element_random(tmp_Zn_2);
    // pai_4_1 = y^a41 y3^a42
    element_pow_zn(tmp_G, pk.get_PKE_CPA_pk()[y], tmp_Zn);
    element_pow_zn(tmp_G_2, pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], tmp_Zn_2);
    element_mul(tmp_G, tmp_G, tmp_G_2);
    h_p.get_r().get_pai().set(pai_4_1, tmp_G);
    // b4
    element_div(tmp_G_2, h_p.get_r().get_c1()[PKE_CPA_AMV_2017::c2_], h_p.get_r().get_c2()[PKE_CCA_AMV_2017::c3_]);
    HASH::hash(tmp_Zn_3, tmp_G_2, pk.get_PKE_CPA_pk()[y], pk.get_PKE_CCA_pk()[PKE_CCA_AMV_2017::y3], tmp_G);
    // y41 = b4rho1 + a41
    element_mul(tmp_Zn_4, tmp_Zn_3, h_p.get_r().get_c1()[PKE_CPA_AMV_2017::rho]);
    element_add(tmp_Zn_4, tmp_Zn_4, tmp_Zn);
    h_p.get_r().get_pai().set(pai_4_2, tmp_Zn_4);
    // y42 = -b4rho2 + a42
    element_mul(tmp_Zn_4, tmp_Zn_3, h_p.get_r().get_c2()[PKE_CCA_AMV_2017::rho]);
    element_sub(tmp_Zn_4, tmp_Zn_2, tmp_Zn_4);
    h_p.get_r().get_pai().set(pai_4_3, tmp_Zn_4);

}

bool CH_AMV_2017::Verify(CH_AMV_2017_h &h_p, element_t m_p, CH_AMV_2017_pk &pk){
    return Check(h_p, m_p, pk);
}


CH_AMV_2017::~CH_AMV_2017(){
    element_clear(tmp_G);
    element_clear(tmp_G_2);
    element_clear(tmp_G_3);
    element_clear(tmp_G_4);
    element_clear(tmp_Zn);
    element_clear(tmp_Zn_2);
    element_clear(tmp_Zn_3);
    element_clear(tmp_Zn_4);

    element_clear(G1);
    element_clear(Zn);
}