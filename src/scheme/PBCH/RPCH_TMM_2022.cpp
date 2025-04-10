#include "scheme/PBCH/RPCH_TMM_2022.h"

RPCH_TMM_2022::RPCH_TMM_2022(int curve, bool swap): PbcScheme(curve){
    this->swap = swap;
    if(swap){
        element_init_G1(G2, pairing);
        element_init_G2(G1, pairing);
    }else{
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    rabe.init(G1, G2, GT, Zn, swap, true);

    element_init_same_as(this->s1, Zn);
    element_init_same_as(this->s2, Zn);
    element_init_same_as(this->K, GT);
    element_init_same_as(this->R, Zn);

    element_init_same_as(this->tmp_G, G1);
    element_init_same_as(this->tmp_G_2, G1);
    element_init_same_as(this->tmp_Zn, Zn);
    element_init_same_as(this->tmp_Zn_2, Zn);
    element_init_same_as(this->tmp_Zn_3, Zn);
}

/**
 * @brief 
 * 
 * @param  skRPCH[out]  
 * @param  pkRPCH[out]  
 * @param  rl[out]      
 * @param  st[out]      
 * @param  n[in]        
 * 
 */
void RPCH_TMM_2022::SetUp(RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_RevokedPresonList &rl, RPCH_TMM_2022_Binary_tree &st, int n) {
    rabe.Setup(pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), rl.get_rl(), st.get_st(), n);

    element_random(tmp_Zn);
    skRPCH.get_skCHET().set(x, tmp_Zn);
    // y = g^x
    element_pow_zn(tmp_G, pkRPCH.get_mpkRABE()[RABE_TMM::g], skRPCH.get_skCHET()[x]);
    pkRPCH.get_pkCHET().set(y, tmp_G);
}

/**
 * @brief 
 * 
 * @param  skidRPCH[out]   
 * @param  pkRPCH[in]     
 * @param  skRPCH[in]     
 * @param  st[in]         
 * @param  attr_list[in]  
 * @param  id[in]          
 * @param  re_time[in]     
 * 
 */
void RPCH_TMM_2022::KeyGen(RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_sk &skRPCH, RPCH_TMM_2022_Binary_tree &st, std::vector<std::string> &attr_list, element_t id, time_t re_time) {
    rabe.KGen(skidRPCH.get_skidRABE(), st.get_st(), pkRPCH.get_mpkRABE(), skRPCH.get_mskRABE(), attr_list, id, re_time);
    skidRPCH.get_skCHET().set(x, skRPCH.get_skCHET()[x]);
}


/**
 * @brief 
 * 
 * @param  kut[out]     
 * @param  pkRPCH[in]  
 * @param  st[in]      
 * @param  rl[in]      
 * @param  t[in]        
 * 
 */
void RPCH_TMM_2022::KUpt(RPCH_TMM_2022_kut &kut, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_Binary_tree &st, RPCH_TMM_2022_RevokedPresonList &rl, time_t t){
    rabe.KUpt(kut.get_kut(), pkRPCH.get_mpkRABE(), st.get_st(), rl.get_rl(), t);
}

/**
 * @brief 
 * 
 * @param  dkidtRPCH[out]  
 * @param  pkRPCH[in]     
 * @param  skidRPCH[in]   
 * @param  kut[in]        
 * 
 */
void RPCH_TMM_2022::DKGen(RPCH_TMM_2022_dkidt &dkidtRPCH, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_skid &skidRPCH, RPCH_TMM_2022_kut &kut){
    rabe.DKGen(dkidtRPCH.get_dkidtRABE(), pkRPCH.get_mpkRABE(), skidRPCH.get_skidRABE(), kut.get_kut());
    dkidtRPCH.get_skCHET().init(1);
    dkidtRPCH.get_skCHET().set(x, skidRPCH.get_skCHET()[x]);
}

/**
 * @brief 
 * 
 * @param  rl[out]  
 * @param  id[in]   
 * @param  t[in]    
 * 
 */
void RPCH_TMM_2022::Rev(RPCH_TMM_2022_RevokedPresonList &rl, element_t id, time_t t){
    rabe.Rev(rl.get_rl(), id, t);
}

/**
 * @brief 
 * 
 * @param  h[out]       
 * @param  r[out]       
 * @param  m[in]        
 * @param  pkRPCH[in]  
 * @param  MSP[in]     
 * @param  t[in]        
 * 
 */
void RPCH_TMM_2022::Hash(RPCH_TMM_2022_h &h,  RPCH_TMM_2022_r &r, element_t m, RPCH_TMM_2022_pk &pkRPCH, Element_t_matrix *MSP, time_t t) {
    // r
    element_random(tmp_Zn);
    r.get_r().set(r1, tmp_Zn);

    element_random(this->R);
    // h = g^R
    element_pow_zn(tmp_G, pkRPCH.get_mpkRABE()[RABE_TMM::g], this->R);
    r.get_r().set(h1, tmp_G);

    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, tmp_G, tmp_Zn);
    element_pow_zn(this->tmp_G_2, pkRPCH.get_pkCHET()[y], m);
    element_mul(tmp_G, this->tmp_G, this->tmp_G_2);
    h.get_h().set(b, tmp_G);

    element_random(this->s1);
    element_random(this->s2);

    this->rabe.Enc(r.get_C(), pkRPCH.get_mpkRABE(), this->R, MSP, t, this->s1, this->s2);
}

/**
 * @brief 
 * 
 * @param  h[in]       
 * @param  r[in]       
 * @param  m[in]        
 * @param  pkRPCH[in]  
 * 
 * @return 
 */
bool RPCH_TMM_2022::Check(RPCH_TMM_2022_h &h, RPCH_TMM_2022_r &r, element_t m, RPCH_TMM_2022_pk &pkRPCH) {
    // CH b = pk^m * h^r
    element_pow_zn(this->tmp_G, r.get_r()[h1], r.get_r()[r1]);
    element_pow_zn(this->tmp_G_2, pkRPCH.get_pkCHET()[y], m);
    element_mul(this->tmp_G, this->tmp_G, this->tmp_G_2);
    
    return element_cmp(h.get_h().get(b), this->tmp_G) == 0;
}

/**
 * @brief 
 * 
 * @param  r_p[out]        
 * @param  m_p[in]         
 * @param  h[in]          
 * @param  r[in]          
 * @param  m[in]           
 * @param  pkRPCH[in]     
 * @param  dkidtRPCH[in]  
 * @param  MSP[in]        
 * 
 */
void RPCH_TMM_2022::Adapt(RPCH_TMM_2022_r &r_p, element_t m_p, RPCH_TMM_2022_h &h, RPCH_TMM_2022_r &r, element_t m, RPCH_TMM_2022_pk &pkRPCH, RPCH_TMM_2022_dkidt &dkidtRPCH, Element_t_matrix *MSP) {
    // Check
    if (!this->Check(h, r, m, pkRPCH)) {
        throw std::runtime_error("Hash Check failed");
        return;
    }

    rabe.Dec(this->R, pkRPCH.get_mpkRABE(), r.get_C(), dkidtRPCH.get_dkidtRABE(), MSP);
    
    // r' = r + (m - m')*sk/R
    element_sub(this->tmp_Zn, m, m_p);
    element_mul(this->tmp_Zn_2, this->tmp_Zn, dkidtRPCH.get_skCHET().get(x));
    element_div(this->tmp_Zn_3, this->tmp_Zn_2, this->R);
    element_add(tmp_Zn, r.get_r().get(r1), this->tmp_Zn_3);
    r_p.get_r().set(r1, tmp_Zn);
    r_p.get_r().set(h1, r.get_r().get(h1));
}

/**
 * @brief 
 * 
 * @param  h[in]       
 * @param  r_p[in]     
 * @param  m_p[in]      
 * @param  pkRPCH[in]  
 * 
 * @return 
 */
bool RPCH_TMM_2022::Verify(RPCH_TMM_2022_h &h, RPCH_TMM_2022_r &r_p, element_t m_p, RPCH_TMM_2022_pk &pkRPCH) {
    return this->Check(h, r_p, m_p, pkRPCH);
}

Element_t_matrix* RPCH_TMM_2022::ComputeMSP(const std::string &policy_str){
    return rabe.ComputeMSP(policy_str);
}

RPCH_TMM_2022::~RPCH_TMM_2022() {
    element_s *clear_list[] = {s1, s2, K, R, 
        tmp_G, tmp_G_2, tmp_Zn, tmp_Zn_2, tmp_Zn_3};
    for(int i=0;i<sizeof(clear_list)/sizeof(clear_list[0]);i++){
        element_clear(clear_list[i]);
    }
}