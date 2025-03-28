#include "ABE/MA_ABE.h"

MA_ABE::MA_ABE(int curve): PbcScheme(curve){
    element_init_G1(G1, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);

    initTmp();
}

void MA_ABE::init(element_t _G1, element_t _GT, element_t _Zn){
    element_init_same_as(G1, _G1);
    element_init_same_as(GT, _GT);
    element_init_same_as(Zn, _Zn);

    initTmp();
}

void MA_ABE::initTmp(){
    element_init_same_as(this->z, Zn);
    element_init_same_as(this->tmp_G, G1);
    element_init_same_as(this->tmp_G_2, G1);
    element_init_same_as(this->tmp_G_3, G1);
    element_init_same_as(this->tmp_G_4, G1);
    element_init_same_as(this->tmp_GT, GT);
    element_init_same_as(this->tmp_GT_2, GT);
    element_init_same_as(this->tmp_GT_3, GT);
    element_init_same_as(this->tmp_Zn, Zn);
}

/**
 * @brief 
 * 
 * @param  gpk[out]  global public key
 * 
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk &gpk){
    element_random(tmp_G);
    gpk.set(g, tmp_G);
    element_pairing(tmp_GT, gpk[g], gpk[g]);
    gpk.set(egg, tmp_GT);
}

/**
 * @brief 
 * 
 * @param  gpk[out]  global public key
 * @param  _g[in]    generator
 * 
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk &gpk, element_t _g){    
    gpk.set(g, _g);
    element_pairing(tmp_GT, gpk[g], gpk[g]);
    gpk.set(egg, tmp_GT);
}

/**
 * @brief 
 * 
 * @param  pkTheta[out]  public key of theta
 * @param  skTheta[out]  secret key of theta
 * @param  gpk[in]       global public key
 * @param  A[in]         an attribute of the authority
 * 
 */
void MA_ABE::AuthSetup(MA_ABE_pkTheta &pkTheta, MA_ABE_skTheta &skTheta, MA_ABE_gpk &gpk, std::string A){
    element_random(tmp_Zn);
    skTheta.set(aTheta, tmp_Zn);
    element_random(tmp_Zn);
    skTheta.set(yTheta, tmp_Zn);
    
    // pkTheta_1 = e(g,g)^aTheta
    element_pow_zn(tmp_GT, gpk[egg], skTheta[aTheta]);
    pkTheta.set(pkTheta_1, tmp_GT);
    // pkTheta_2 = g^yTheta
    element_pow_zn(tmp_G, gpk[g], skTheta[yTheta]);
    pkTheta.set(pkTheta_2, tmp_G);

    pkTheta.set_A(A);
}

/**
 * @brief HGID(bit, gid) -> G
 * 
 * @param  res[out]  
 * @param  bit[in]  0 or 1
 * @param  gid[in]  
 * 
 */
void MA_ABE::HGID(element_t res, bool bit, std::string gid){
    std::string m = std::to_string(bit) + gid;
    HASH::hash(res, m);
}

/**
 * @brief Hu(u) -> G
 * 
 * @param  res[out]  
 * @param  u[in]    
 * 
 */
void MA_ABE::Hu(element_t res, std::string u){
    HASH::hash(res, u);
}

/**
 * @brief Ht(rt, A) -> G
 * 
 * @param  res[out]  
 * @param  rt[in]   
 * @param  A[in]    
 * 
 */
void MA_ABE::Ht(element_t res, element_t rt, std::string A){
    HASH::hash(res, rt, A);
}

/**
 * @brief 
 * 
 * @param  skgidA[out]   secret key of gid and A
 * @param  gpk[in]      global public key
 * @param  skTheta[in]  secret key of theta
 * @param  gid[in]       a global identifier
 * @param  A[in]         an attribute
 * 
 */
void MA_ABE::KeyGen(MA_ABE_skgidA &skgidA, MA_ABE_gpk &gpk, MA_ABE_skTheta &skTheta, std::string gid, std::string A){  
    // t
    element_random(this->tmp_Zn);
    // g^aTheta * HGID(0,gid)^yTheta * Hu(A)^t
    element_pow_zn(this->tmp_G, gpk[g], skTheta[aTheta]);
    this->HGID(this->tmp_G_2, 0, gid);
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, skTheta[yTheta]);
    this->Hu(this->tmp_G_3, A);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn);
    element_mul(tmp_G_4, this->tmp_G, this->tmp_G_2);
    element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
    skgidA.set(skgidA_0, tmp_G_4);

    // g^t
    element_pow_zn(tmp_G_4, gpk[g], this->tmp_Zn);
    skgidA.set(skgidA_1, tmp_G_4);

    skgidA.set_gid(gid);
    skgidA.set_A(A);
}

/**
 * @brief 
 * 
 * @param  C[out]          ciphertext
 * @param  m[in]           message
 * @param  rt[in]          random value
 * @param  gpk[in]        global public key
 * @param  pkThetas[in]   public keys of the authorities
 * @param  MSP[in]        
 * @param  policy_str[in]  access policy
 * 
 */
void MA_ABE::Encrypt(MA_ABE_ciphertext &C, element_t m, element_t rt, MA_ABE_gpk &gpk, std::vector<MA_ABE_pkTheta *> &pkThetas, Element_t_matrix *MSP, std::string policy_str){    
    unsigned long int rows = MSP->row();
    unsigned long int cols = MSP->col();

    C.setPolicy(policy_str);

    // z = Ht(rt, policy)
    this->Ht(this->z, rt, policy_str);

    // c0 = m * e(g,g)^z
    element_pow_zn(tmp_GT, gpk[egg], this->z);
    element_mul(tmp_GT, tmp_GT, m);
    C.get_ct_0().set(0, tmp_GT);

    // compute ci
    // ti = Ht(rt, policy, 0, i)
    element_s ** ti = new element_s*[rows];
    for(int i = 0;i<rows;i++){
        element_s *tmp_ti = new element_s();
        element_init_same_as(tmp_ti, this->Zn);
        std::string str_ti = policy_str + "0" + std::to_string(i);
        this->Ht(tmp_ti, rt, str_ti);
        ti[i] = tmp_ti;
    }
    // vi = Ht(rt, policy, 1, i), wi = Ht(rt, policy, 2, i)
    element_s ** vi = new element_s*[cols];
    element_s ** wi = new element_s*[cols];
    for(int i = 0;i<cols;i++){
        element_s *tmp_vi = new element_s();
        element_init_same_as(tmp_vi, this->Zn);
        std::string str_vi = policy_str + "1" + std::to_string(i);
        this->Ht(tmp_vi, rt, str_vi);
        vi[i] = tmp_vi;

        element_s *tmp_wi = new element_s();
        element_init_same_as(tmp_wi, this->Zn);
        std::string str_wi = policy_str + "2" + std::to_string(i);
        this->Ht(tmp_wi, rt, str_wi);
        wi[i] = tmp_wi;
    }
    // v = (z, v2, ..., vl2)T
    element_s ** v = new element_s*[cols];
    for(int i=0;i<cols;i++){
        element_s *tmp_v = new element_s();
        element_init_same_as(tmp_v, this->Zn);
        if(i==0){
            element_set(tmp_v, this->z);
        }else{
            element_set(tmp_v, vi[i]);
        }
        v[i] = tmp_v;
    }
    // w = (0, w2, ..., wl2)T
    element_s ** w = new element_s*[cols];
    for(int i=0;i<cols;i++){
        element_s *tmp_w = new element_s();
        element_init_same_as(tmp_w, this->Zn);
        if(i==0){
            element_set0(tmp_w);
        }else{
            element_set(tmp_w, wi[i]);
        }
        w[i] = tmp_w;
    }
    // lamuda = M * v
    element_s** lamuda = new element_s*[rows];
    for(int i=0;i<rows;i++){
        element_s *tmp_lamuda = new element_s();
        element_init_same_as(tmp_lamuda, this->Zn);
        element_set0(tmp_lamuda);
        for(int j=0;j<cols;j++){
            element_mul(this->tmp_Zn, MSP->getElement(i, j), v[j]);
            element_add(tmp_lamuda, tmp_lamuda, this->tmp_Zn);
        }
        lamuda[i] = tmp_lamuda;
    }
    // w = M * w
    element_s** w_tmp = new element_s*[rows];
    for(int i=0;i<rows;i++){
        element_s *tmp_w = new element_s();
        element_init_same_as(tmp_w, this->Zn);
        element_set0(tmp_w);
        for(int j=0;j<cols;j++){
            element_mul(this->tmp_Zn, MSP->getElement(i, j), w[j]);
            element_add(tmp_w, tmp_w, this->tmp_Zn);
        }
        w_tmp[i] = tmp_w;
    }


    for(int i=0;i<rows;i++){
        // ci_1 = e(g,g)^lamuda_i * e(g,g)^(a*ti)
        std::string attr = MSP->getName(i);
        element_pow_zn(tmp_GT, gpk[egg], lamuda[i]);
        for(int j=0;j<pkThetas.size();j++){
            if(pkThetas[j]->get_A() == attr){
                element_pow_zn(tmp_GT_2, pkThetas[j]->get(pkTheta_1), ti[i]);
                element_pow_zn(tmp_G, pkThetas[j]->get(pkTheta_2), ti[i]);
                break;
            }
        }
        element_mul(tmp_GT, tmp_GT, tmp_GT_2);
        
        PbcElements tmp_ci;
        tmp_ci.init(4);

        tmp_ci.set(ct_1, tmp_GT);

        // ci_2 = g^(-ti)
        element_neg(tmp_Zn, ti[i]);
        element_pow_zn(tmp_G_2, gpk[g], tmp_Zn);
        tmp_ci.set(ct_2, tmp_G_2);

        // ci_3 = g^(y*ti) * g^wi
        element_pow_zn(tmp_G_2, gpk[g], w_tmp[i]);
        element_mul(tmp_G_2, tmp_G_2, tmp_G);
        tmp_ci.set(ct_3, tmp_G_2);

        // ci_4 = Hu(π(i))^ti
        this->Hu(tmp_G, attr);
        element_pow_zn(tmp_G_2, tmp_G, ti[i]);
        tmp_ci.set(ct_4, tmp_G_2);

        C.get_ct_i().push_back(tmp_ci);
    }

    // free
    for(int i=0;i<rows;i++){
        element_clear(ti[i]);
        element_clear(lamuda[i]);
        element_clear(w_tmp[i]);
    }
    for(int i=0;i<cols;i++){
        element_clear(vi[i]);
        element_clear(wi[i]);
        element_clear(v[i]);
        element_clear(w[i]);
    }
    delete[] ti;
    delete[] lamuda;
    delete[] w_tmp;
    delete[] vi;
    delete[] wi;
    delete[] v;
    delete[] w;
}

/**
 * @brief 
 * 
 * @param  res[out]     decrypted message
 * @param  skgidAs[in]  secret keys of the authorities
 * @param  C[in]        ciphertext
 * @param  MSP[in]      
 * 
 */
void MA_ABE::Decrypt(element_t res, std::vector<MA_ABE_skgidA *> &skgidAs, MA_ABE_ciphertext &C, Element_t_matrix *MSP){
    // compute Yi
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = C.get_ct_i().size();
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs.size();k++){
            if(skgidAs[k]->get_A() == MSP->getName(i)){
                Element_t_vector *v = new Element_t_vector();
                for (signed long int j = 0; j < MSP->col(); ++j) {
                    v->pushBack(MSP->getElement(i, j));
                }
                attributesMatrix->pushBack(v);
                break;
            }
        }
    }
    // get inverse matrix
    Element_t_matrix* inverse_attributesMatrix = attributesMatrix->inverse();

    unsigned long int r = inverse_attributesMatrix->row();
    unsigned long int c = inverse_attributesMatrix->col();

    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x= new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if (-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }

    element_set1(tmp_GT_3);
    int count = 0;
    for(int i=0;i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs.size();k++){
            if(skgidAs[k]->get_A() == MSP->getName(i)){
                element_set(tmp_GT, C.get_ct_i(i)[ct_1]);
                // e(skgidA_0,ci_2)
                element_pairing(tmp_GT_2, skgidAs[k]->get(skgidA_0), C.get_ct_i(i)[ct_2]);
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);
                // e(HGID(0,gid),ci_3)
                this->HGID(tmp_G, 0, skgidAs[k]->get_gid());
                element_pairing(tmp_GT_2, tmp_G, C.get_ct_i(i)[ct_3]);
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);
                // e(skgidA_1,ci_4)
                element_pairing(tmp_GT_2, skgidAs[k]->get(skgidA_1), C.get_ct_i(i)[ct_4]);
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);

                // tmp_GT^yi
                element_pow_zn(tmp_GT, tmp_GT, x->getElement(count++));

                element_mul(tmp_GT_3, tmp_GT_3, tmp_GT);
                break;
            }
        }
    }
    // res = c0 / tmp_GT_3
    element_div(res, C.get_ct_0()[0], tmp_GT_3);
}

MA_ABE::~MA_ABE(){
    element_s *clear_list[] = {z, tmp_G, tmp_G_2, tmp_G_3, tmp_G_4, tmp_GT, tmp_GT_2, tmp_GT_3, tmp_Zn, G1, GT, Zn};
    for(int i=0;i<sizeof(clear_list)/sizeof(element_s*);i++){
        element_clear(clear_list[i]);
    }
}