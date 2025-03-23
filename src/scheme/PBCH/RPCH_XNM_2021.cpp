#include <scheme/PBCH/RPCH_XNM_2021.h>

RPCH_XNM_2021::RPCH_XNM_2021(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G2(G1, pairing);
        element_init_G1(G2, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    rabe.init(G1, G2, GT, Zn, swap);

    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
    element_init_same_as(this->K, GT);
}

/**
 * input : k, n
 * output: ppRPCH, skRPCH, pkRPCH, _rl, _st
 */
void RPCH_XNM_2021::SetUp(RPCH_XNM_2021_pp &ppRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_RevokedPresonList &rl, RPCH_XNM_2022_Binary_tree &st, int k, int n) {
    ppRPCH.get_ppCHET().setK(k);
    ch.KeyGen(pkRPCH.get_pkCHET(), skRPCH.get_skCHET(), ppRPCH.get_ppCHET());

    rabe.Setup(pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), rl.get_rl(), st.get_st(), n);
}

/**
 * input : pkRPCH, skRPCH, _st, id, attr_list
 * output: skidRPCH
 */
void RPCH_XNM_2021::KeyGen(RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_sk &skRPCH, RPCH_XNM_2022_Binary_tree &st, std::vector<std::string> &attr_list, element_t id, time_t re_time){
    rabe.KGen(skidRPCH.get_skidRABE(), st.get_st(), pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), attr_list, id, re_time);
    skidRPCH.get_skCHET().copyFrom(skRPCH.get_skCHET());
}

/**
 * input : pkRPCH, _st, _rl, t
 * output: kut
 */
void RPCH_XNM_2021::KUpt(RPCH_XNM_2022_kut &kut, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2022_Binary_tree &st, RPCH_XNM_2021_RevokedPresonList &rl, time_t t){
    rabe.KUpt(kut.get_kut(), pkRPCH.get_mpkRABE(), st.get_st(), rl.get_rl(), t);
}

/**
 * input : pkRPCH, skidRPCH, kut
 * output: dkidtRPCH
 */
void RPCH_XNM_2021::DKGen(RPCH_XNM_2021_dkidt &dkidtRPCH, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_skid &skidRPCH, RPCH_XNM_2022_kut &kut){    
    rabe.DKGen(dkidtRPCH.get_dkidtRABE(), pkRPCH.get_mpkRABE(), skidRPCH.get_skidRABE(), kut.get_kut());
    dkidtRPCH.get_skCHET().copyFrom(skidRPCH.get_skCHET());
}

/**
 * input : _rl, id, t
 */
void RPCH_XNM_2021::Rev(RPCH_XNM_2021_RevokedPresonList &rl, element_t id, time_t t){
    this->rabe.Rev(rl.get_rl(), id, t);
}


/**
 * input : pkRPCH, policy_str, t
 * output: h
 */
void RPCH_XNM_2021::Hash(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH, Element_t_matrix *MSP, time_t t, RPCH_XNM_2021_pp &ppRPCH) {
    CH_ET_BC_CDK_2017_etd etd;
    
    ch.Hash(h.get_h(), r.get_rCHET(), etd, ppRPCH.get_ppCHET(), pkRPCH.get_pkCHET(), m);

    this->aes.KGen(this->K, 256);

    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(h.get_ct(), pkRPCH.get_mpkRABE(), this->K, MSP, t, this->s1, this->s2);

    // cSE = EncSE(kk, d2)
    int enc_len;
    unsigned char *enc = aes.Enc(&enc_len, K, etd[0]);
    h.set_cSE(enc, enc_len);
    
}

/**
 * input : pkRPCH, h
 * output: bool
 */
bool RPCH_XNM_2021::Check(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH) {
    return ch.Check(h.get_h(), r.get_rCHET(), pkRPCH.get_pkCHET(), m);
}

/**
 * input : pkRPCH, dkidtRPCH, h
 * output: h_p
 */
void RPCH_XNM_2021::Adapt(RPCH_XNM_2021_r &r_p, std::string m_p, RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r, std::string m, RPCH_XNM_2021_pk &pkRPCH, RPCH_XNM_2021_dkidt &dkidtRPCH, Element_t_matrix *MSP) {
    rabe.Dec(this->K, pkRPCH.get_mpkRABE(), h.get_ct(), dkidtRPCH.get_dkidtRABE(), MSP);

    CH_ET_BC_CDK_2017_etd etd;
    // DecSE(kk, ct_) -> d2
    aes.Dec(etd[0], this->K, h.get_cSE(), h.get_cSE_len());

    ch.Adapt(r_p.get_rCHET(), dkidtRPCH.get_skCHET(), etd, pkRPCH.get_pkCHET(), h.get_h(), r.get_rCHET(), m, m_p);
}

/**
 * input : pkPCH, h_p
 * output: bool
 */
bool RPCH_XNM_2021::Verify(RPCH_XNM_2021_h &h, RPCH_XNM_2021_r &r_p, std::string m_p, RPCH_XNM_2021_pk &pkRPCH) {
    return this->Check(h, r_p, m_p, pkRPCH);
}


RPCH_XNM_2021::~RPCH_XNM_2021() {
    element_s *clear_list[] = {s1, s2, K, G1, G2, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}