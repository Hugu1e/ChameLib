#include "ABE/MA_ABE.h"

MA_ABE::MA_ABE(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->z, Zn);
}

/**
 * GlobalSetup() -> gp
 * @param gpk global public key
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk &gpk){
    gpk.init(1);
    
    element_random(tmp_G);
    gpk.set(g, tmp_G);
}
/**
 * GlobalSetup(g) -> gp
 * @param gpk global public key
 * @param g generator g
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk &gpk, element_t _g){
    gpk.init(1);
    
    gpk.set(g, _g);
}

/**
 * AuthSetup(theta) -> (pktheta, sktheta)
 * 
 * @param pkTheta public key of theta
 * @param skTheta secret key of theta
 * @param gpk global public key
 * @param A an attribute of the authority
 */
void MA_ABE::AuthSetup(MA_ABE_pkTheta &pkTheta, MA_ABE_skTheta &skTheta, MA_ABE_gpk &gpk, std::string A){
    pkTheta.init(2);
    skTheta.init(2);
    
    element_random(tmp_Zn);
    skTheta.set(aTheta, tmp_Zn);
    element_random(tmp_Zn);
    skTheta.set(yTheta, tmp_Zn);
    
    // pkTheta_1 = e(g,g)^aTheta
    element_pairing(tmp_GT, gpk[g], gpk[g]);
    element_pow_zn(tmp_GT, tmp_GT, skTheta[aTheta]);
    pkTheta.set(pkTheta_1, tmp_GT);
    // pkTheta_2 = g^yTheta
    element_pow_zn(tmp_G, gpk[g], skTheta[yTheta]);
    pkTheta.set(pkTheta_2, tmp_G);

    pkTheta.set_A(A);
}

/**
 * HGID(bit, gid) -> G
 * @param res 
 * @param bit 0 or 1
 * @param gid
 */
void MA_ABE::HGID(element_t res, bool bit, std::string gid){
    std::string m = std::to_string(bit) + gid;
    HASH::hash(res, m);
}

/**
 * Hu(u) -> G
 * @param res 
 * @param u
 */
void MA_ABE::Hu(element_t res, std::string u){
    HASH::hash(res, u);
}

/**
 * Ht(rt, A) -> G
 * 
 * @param res
 * @param rt
 * @param A
 */
void MA_ABE::Ht(element_t res, element_t rt, std::string A){
    HASH::hash(res, rt, A);
}

/**
 * KeyGen(gpk, skTheta, gid, A) -> skgidA
 * @param skgidA secret key of gid and A
 * @param gpk global public key
 * @param skTheta secret key of theta
 * @param gid a global identifier
 * @param A an attribute
 */
void MA_ABE::KeyGen(MA_ABE_skgidA &skgidA, MA_ABE_gpk &gpk, MA_ABE_skTheta &skTheta, std::string gid, std::string A){
    skgidA.init(2);
    
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
 * Encrypt(gpk, pkThetas, polocy, m) -> c
 * @param C ciphertext
 * @param m message
 * @param rt random value
 * @param gpk global public key
 * @param pkThetas public keys of the authorities
 * @param policy access policy

 */
void MA_ABE::Encrypt(MA_ABE_ciphertext &C, element_t m, element_t rt, MA_ABE_gpk &gpk, std::vector<MA_ABE_pkTheta *> &pkThetas, std::string policy){
    C.get_ct_0().init(1);
    
    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);

    std::vector<std::string>* postfix_expression = pr.infixToPostfix(policy);
    // for(int i = 0;i < postfix_expression->size();i++){
    //     printf("%s ", postfix_expression->at(i).c_str());
    // }
    // printf("\n");

    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    unsigned long int rows = M->row();
    unsigned long int cols = M->col();

    // printf("rows: %ld, cols: %ld\n", rows, cols);
    // for(int i = 0;i < rows;i++){
    //     for(int j = 0;j < cols;j++){
    //         element_printf("%B ", M->getElement(i, j));
    //     }
    //     printf("\n");
    // }

    C.setPolicy(policy);

    // z = Ht(rt, policy)
    this->Ht(this->z, rt, policy);

    // c0 = m * e(g,g)^z
    element_pairing(tmp_GT, gpk[g], gpk[g]);
    element_pow_zn(tmp_GT, tmp_GT, this->z);
    element_mul(tmp_GT, tmp_GT, m);
    C.get_ct_0().set(0, tmp_GT);

    // compute ci
    // ti = Ht(rt, policy, 0, i)
    element_s ** ti = new element_s*[rows];
    for(int i = 0;i<rows;i++){
        element_s *tmp_ti = new element_s();
        element_init_same_as(tmp_ti, this->Zn);
        std::string str_ti = policy + "0" + std::to_string(i);
        this->Ht(tmp_ti, rt, str_ti);
        ti[i] = tmp_ti;
    }
    // vi = Ht(rt, policy, 1, i), wi = Ht(rt, policy, 2, i)
    element_s ** vi = new element_s*[cols];
    element_s ** wi = new element_s*[cols];
    for(int i = 0;i<cols;i++){
        element_s *tmp_vi = new element_s();
        element_init_same_as(tmp_vi, this->Zn);
        std::string str_vi = policy + "1" + std::to_string(i);
        this->Ht(tmp_vi, rt, str_vi);
        vi[i] = tmp_vi;

        element_s *tmp_wi = new element_s();
        element_init_same_as(tmp_wi, this->Zn);
        std::string str_wi = policy + "2" + std::to_string(i);
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
            element_mul(this->tmp_Zn, M->getElement(i, j), v[j]);
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
            element_mul(this->tmp_Zn, M->getElement(i, j), w[j]);
            element_add(tmp_w, tmp_w, this->tmp_Zn);
        }
        w_tmp[i] = tmp_w;
    }


    for(int i=0;i<rows;i++){
        // ci_1 = e(g,g)^lamuda_i * e(g,g)^(a*ti)
        std::string attr = M->getName(i);
        pai[i] = attr;
        element_pairing(tmp_GT, gpk[g], gpk[g]);
        element_pow_zn(tmp_GT, tmp_GT, lamuda[i]);
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
 * Decrypt(skgidAs, c) -> m
 * @param res message
 * @param skgidAs secret keys of the authorities
 * @param C ciphertext
 */
void MA_ABE::Decrypt(element_t res, std::vector<MA_ABE_skgidA *> &skgidAs, MA_ABE_ciphertext &C){
    // compute Yi
    // get original matrix
    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(C.get_policy());
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = C.get_ct_i().size();
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs.size();k++){
            if(skgidAs[k]->get_A() == pai[i]){
                Element_t_vector *v = new Element_t_vector();
                for (signed long int j = 0; j < M->col(); ++j) {
                    v->pushBack(M->getElement(i, j));
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
    // printf("rows: %ld, cols: %ld\n", r, c);
    // for(int i = 0;i < r;i++){
    //     for(int j = 0;j < c;j++){
    //         element_printf("%B ", inverse_attributesMatrix->getElement(i, j));
    //     }
    //     printf("\n");
    // }
    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x= new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if (-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }
    // printf("type: %ld\n", type);
    // // print x
    // printf("Yi:\n");
    // x->printVector();


    element_set1(tmp_GT_3);
    int count = 0;
    for(int i=0;i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs.size();k++){
            if(skgidAs[k]->get_A() == pai[i]){
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
    element_clear(this->z);
}