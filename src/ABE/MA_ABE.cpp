#include <ABE/MA_ABE.h>

MA_ABE::MA_ABE(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn): PbcScheme(_G1, _G2, _GT, _Zn){
    element_init_same_as(this->z, Zn);
}

/**
 * GlobalSetup() -> gp
 * @param gpk global public key
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk *gpk){
    element_random(tmp_G);
    gpk->insertElement("g", "G1", tmp_G);
}
/**
 * GlobalSetup(g) -> gp
 * @param gpk global public key
 * @param g generator g
 */
void MA_ABE::GlobalSetup(MA_ABE_gpk *gpk, element_t g){
    gpk->insertElement("g", "G1", g);
}

/**
 * AuthSetup(theta) -> (pktheta, sktheta)
 * 
 * @param pkTheta public key of theta
 * @param skTheta secret key of theta
 * @param gpk global public key
 * @param A an attribute of the authority
 */
void MA_ABE::AuthSetup(MA_ABE_pkTheta *pkTheta, MA_ABE_skTheta *skTheta, MA_ABE_gpk *gpk, std::string A){
    element_random(tmp_Zn);
    skTheta->insertElement("aTheta", "Zn", tmp_Zn);
    element_random(tmp_Zn);
    skTheta->insertElement("yTheta", "Zn", tmp_Zn);
    
    // pkTheta_1 = e(g,g)^aTheta
    element_pairing(tmp_GT, gpk->getElement("g"), gpk->getElement("g"));
    element_pow_zn(tmp_GT, tmp_GT, skTheta->getElement("aTheta"));
    pkTheta->insertElement("pkTheta_1", "GT", tmp_GT);
    // pkTheta_2 = g^yTheta
    element_pow_zn(tmp_G, gpk->getElement("g"), skTheta->getElement("yTheta"));
    pkTheta->insertElement("pkTheta_2", "G1", tmp_G);

    pkTheta->setA(A);
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
 * KeyGen(gpk, skTheta, gid, A) -> skgidA
 * @param skgidA secret key of gid and A
 * @param gpk global public key
 * @param skTheta secret key of theta
 * @param gid a global identifier
 * @param A an attribute
 */
void MA_ABE::KeyGen(MA_ABE_skgidA *skgidA, MA_ABE_gpk *gpk, MA_ABE_skTheta *skTheta, std::string gid, std::string A){
    // t
    element_random(this->tmp_Zn);
    // g^aTheta * HGID(0,gid)^yTheta * Hu(A)^t
    element_pow_zn(this->tmp_G, gpk->getElement("g"), skTheta->getElement("aTheta"));
    this->HGID(this->tmp_G_2, 0, gid);
    element_pow_zn(this->tmp_G_2, this->tmp_G_2, skTheta->getElement("yTheta"));
    this->Hu(this->tmp_G_3, A);
    element_pow_zn(this->tmp_G_3, this->tmp_G_3, this->tmp_Zn);
    element_mul(tmp_G_4, this->tmp_G, this->tmp_G_2);
    element_mul(tmp_G_4, tmp_G_4, this->tmp_G_3);
    skgidA->insertElement("skgidA_0", "G1", tmp_G_4);

    // g^t
    element_pow_zn(tmp_G_4, gpk->getElement("g"), this->tmp_Zn);
    skgidA->insertElement("skgidA_1", "G1", tmp_G_4);

    skgidA->setGid(gid);
    skgidA->setA(A);
}

/**
 * Encrypt(gpk, pkThetas, polocy, m) -> c
 * @param C ciphertext
 * @param gpk global public key
 * @param pkThetas public keys of the authorities
 * @param policy access policy
 * @param m message
 */
void MA_ABE::Encrypt(MA_ABE_ciphertext *C, MA_ABE_gpk *gpk, std::vector<MA_ABE_pkTheta *> *pkThetas, std::string policy, element_t m){
    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);

    std::vector<std::string>* postfix_expression = pr.infixToPostfix(policy);
    for(int i = 0;i < postfix_expression->size();i++){
        printf("%s ", postfix_expression->at(i).c_str());
    }
    printf("\n");

    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    unsigned long int rows = M->row();
    unsigned long int cols = M->col();

    printf("rows: %ld, cols: %ld\n", rows, cols);
    for(int i = 0;i < rows;i++){
        for(int j = 0;j < cols;j++){
            element_printf("%B ", M->getElement(i, j));
        }
        printf("\n");
    }

    C->setPolicy(policy);

    // z = H(policy)
    this->Hu(this->z, policy);

    // c0 = m * e(g,g)^z
    element_pairing(tmp_GT, gpk->getElement("g"), gpk->getElement("g"));
    element_pow_zn(tmp_GT, tmp_GT, this->z);
    element_mul(tmp_GT, tmp_GT, m);
    C->getC0()->insertElement("c0", "GT", tmp_GT);

    // compute ci
    // ti = H(policy, 0, i)
    std::vector<element_s *> ti;
    for(int i = 0;i<rows;i++){
        element_s *tmp_ti = new element_s();
        element_init_same_as(tmp_ti, this->Zn);
        std::string str_ti = policy + "0" + std::to_string(i);
        this->Hu(tmp_ti, str_ti);
        ti.push_back(tmp_ti);
    }
    // vi = H(policy, 1, i), wi = H(policy, 2, i)
    std::vector<element_s *> vi;
    std::vector<element_s *> wi;
    for(int i = 0;i<cols;i++){
        element_s *tmp_vi = new element_s();
        element_init_same_as(tmp_vi, this->Zn);
        std::string str_vi = policy + "1" + std::to_string(i);
        this->Hu(tmp_vi, str_vi);
        vi.push_back(tmp_vi);

        element_s *tmp_wi = new element_s();
        element_init_same_as(tmp_wi, this->Zn);
        std::string str_wi = policy + "2" + std::to_string(i);
        this->Hu(tmp_wi, str_wi);
        wi.push_back(tmp_wi);
    }
    // v = (z, v2, ..., vl2)T
    std::vector<element_s *> v;
    for(int i=0;i<cols;i++){
        element_s *tmp_v = new element_s();
        element_init_same_as(tmp_v, this->Zn);
        if(i==0){
            element_set(tmp_v, this->z);
        }else{
            element_set(tmp_v, vi[i]);
        }
        v.push_back(tmp_v);
    }
    // w = (0, w2, ..., wl2)T
    std::vector<element_s *> w;
    for(int i=0;i<cols;i++){
        element_s *tmp_w = new element_s();
        element_init_same_as(tmp_w, this->Zn);
        if(i==0){
            element_set0(tmp_w);
        }else{
            element_set(tmp_w, wi[i]);
        }
        w.push_back(tmp_w);
    }
    // lamuda = M * v
    std::vector<element_s *> lamuda;
    for(int i=0;i<rows;i++){
        element_s *tmp_lamuda = new element_s();
        element_init_same_as(tmp_lamuda, this->Zn);
        element_set0(tmp_lamuda);
        for(int j=0;j<cols;j++){
            element_mul(this->tmp_Zn, M->getElement(i, j), v[j]);
            element_add(tmp_lamuda, tmp_lamuda, this->tmp_Zn);
        }
        lamuda.push_back(tmp_lamuda);
    }
    // w = M * w
    std::vector<element_s *> w_tmp;
    for(int i=0;i<rows;i++){
        element_s *tmp_w = new element_s();
        element_init_same_as(tmp_w, this->Zn);
        element_set0(tmp_w);
        for(int j=0;j<cols;j++){
            element_mul(this->tmp_Zn, M->getElement(i, j), w[j]);
            element_add(tmp_w, tmp_w, this->tmp_Zn);
        }
        w_tmp.push_back(tmp_w);
    }


    for(int i=0;i<rows;i++){
        // ci_1 = e(g,g)^lamuda_i * e(g,g)^(a*ti)
        std::string attr = M->getName(i);
        pai[i] = attr;
        element_pairing(tmp_GT, gpk->getElement("g"), gpk->getElement("g"));
        element_pow_zn(tmp_GT, tmp_GT, lamuda[i]);
        for(int j=0;j<pkThetas->size();j++){
            if(pkThetas->at(j)->getA() == attr){
                element_pow_zn(tmp_GT_2, pkThetas->at(j)->getElement("pkTheta_1"), ti[i]);
                element_pow_zn(tmp_G, pkThetas->at(j)->getElement("pkTheta_2"), ti[i]);
                break;
            }
        }
        element_mul(tmp_GT, tmp_GT, tmp_GT_2);
        
        PbcElements tmp_ci;
        tmp_ci.insertElement("ci_1", "GT", tmp_GT);

        // ci_2 = g^(-ti)
        element_neg(tmp_Zn, ti[i]);
        element_pow_zn(tmp_G_2, gpk->getElement("g"), tmp_Zn);
        tmp_ci.insertElement("ci_2", "G1", tmp_G_2);

        // ci_3 = g^(y*ti) * g^wi
        element_pow_zn(tmp_G_2, gpk->getElement("g"), w_tmp[i]);
        element_mul(tmp_G_2, tmp_G_2, tmp_G);
        tmp_ci.insertElement("ci_3", "G1", tmp_G_2);

        // ci_4 = Hu(π(i))^ti
        this->Hu(tmp_G, attr);
        element_pow_zn(tmp_G_2, tmp_G, ti[i]);
        tmp_ci.insertElement("ci_4", "G1", tmp_G_2);

        C->getCi()->push_back(tmp_ci);
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
}

/**
 * Decrypt(skgidAs, c) -> m
 * @param res message
 * @param skgidAs secret keys of the authorities
 * @param C ciphertext
 */
void MA_ABE::Decrypt(element_t res, std::vector<MA_ABE_skgidA *> *skgidAs, MA_ABE_ciphertext *C){
    // compute Yi
    // get original matrix
    Policy_resolution pr;
    Policy_generation pg;
    element_random(this->tmp_Zn);
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(C->getPolicy());
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, this->tmp_Zn);
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* M = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
    // get matrix with attributes
    Element_t_matrix* attributesMatrix = new Element_t_matrix();
    unsigned long int rows = C->getCi()->size();
    for(unsigned long int i=0; i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs->size();k++){
            if(skgidAs->at(k)->getA() == pai[i]){
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
    printf("rows: %ld, cols: %ld\n", r, c);
    for(int i = 0;i < r;i++){
        for(int j = 0;j < c;j++){
            element_printf("%B ", inverse_attributesMatrix->getElement(i, j));
        }
        printf("\n");
    }
    Element_t_vector* unit = inverse_attributesMatrix->getCoordinateAxisUnitVector();

    Element_t_vector* x= new Element_t_vector(inverse_attributesMatrix->col(), inverse_attributesMatrix->getElement(0, 0));

    signed long int type = Element_t_matrix::gaussElimination(x, inverse_attributesMatrix, unit);
    if (-1 == type) {
        throw std::runtime_error("POLICY_NOT_SATISFIED");
    }
    printf("type: %ld\n", type);
    // print x
    printf("Yi:\n");
    x->printVector();


    element_set1(tmp_GT_3);
    int count = 0;
    for(int i=0;i<rows;i++){
        // judge whether the attribute of the policy is in the skgidAs
        for(int k=0;k<skgidAs->size();k++){
            if(skgidAs->at(k)->getA() == pai[i]){
                element_set(tmp_GT, C->getCi(i)->getElement("ci_1"));
                // e(skgidA_0,ci_2)
                element_pairing(tmp_GT_2, skgidAs->at(k)->getElement("skgidA_0"), C->getCi(i)->getElement("ci_2"));
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);
                // e(HGID(0,gid),ci_3)
                this->HGID(tmp_G, 0, skgidAs->at(k)->getGid());
                element_pairing(tmp_GT_2, tmp_G, C->getCi(i)->getElement("ci_3"));
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);
                // e(skgidA_1,ci_4)
                element_pairing(tmp_GT_2, skgidAs->at(k)->getElement("skgidA_1"), C->getCi(i)->getElement("ci_4"));
                element_mul(tmp_GT, tmp_GT, tmp_GT_2);

                // tmp_GT^yi
                element_pow_zn(tmp_GT, tmp_GT, x->getElement(count++));

                element_mul(tmp_GT_3, tmp_GT_3, tmp_GT);
                break;
            }
        }
    }
    // res = c0 / tmp_GT_3
    element_div(res, C->getC0()->getElement("c0"), tmp_GT_3);
}

MA_ABE::~MA_ABE(){
    element_clear(this->z);
}