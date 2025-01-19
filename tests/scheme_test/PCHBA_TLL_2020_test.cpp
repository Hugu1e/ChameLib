#include <scheme/PCHBA_TLL_2020.h>
#include "curve/params.h"
#include <chrono>
#include <iostream>
#include <cstring>
#include "pbc/pbc.h"

int test_result = 1;

FILE *out = NULL;

int turns = 0, turns_pg = 1, turns_kg = 1, turns_h = 1, turns_f = 1;

const bool out_file = true, visiable = false;

pbc_param_t par;
pairing_t pairing;
element_t G1, G2, GT, Zp;

std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
const string POLICY = "(ONE&THREE)&(TWO|FOUR)";
const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

const int K = 10;
const int I = 5;  // modifier
const int J = 5;  // owner
PCHBA_TLL_2020::skPCHBA skPCHBA;
PCHBA_TLL_2020::pkPCHBA pkPCHBA;
ABET::ID ID;
PCHBA_TLL_2020::sksPCHBA sksPCHBA;
ABET::ciphertext C,C_p;

element_t m,m_p;
element_t p,h_,b,c,epk,sigma;
element_t p_p,c_p,epk_p,sigma_p;



std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;

void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);

    skPCHBA.Init(&G1, &G2, &Zp, K);
    pkPCHBA.Init(&G1, &G2, &GT, K);
    ID.Init(&Zp, K);
    for(int i = 1;i<=K;i++){
        element_random(*ID.Ik[i]);
    }
    sksPCHBA.Init(&G1, &G2, &Zp, SIZE_OF_ATTR, I);
    C.Init(&G1, &G2, &GT, &Zp, SIZE_OF_POLICY);
    C_p.Init(&G1, &G2, &GT, &Zp, SIZE_OF_POLICY);
    
    element_init_same_as(m, Zp);
    element_init_same_as(p, G2);
    element_init_same_as(h_, G2);
    element_init_same_as(b, G2);
    element_init_same_as(c, G2);
    element_init_same_as(epk, G1);
    element_init_same_as(sigma, Zp);

    element_init_same_as(m_p, Zp);
    element_init_same_as(p_p, G2);
    element_init_same_as(c_p, G2);
    element_init_same_as(epk_p, G1);
    element_init_same_as(sigma_p, Zp);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void PCHBA_TLL_2020_test() {
    printf("PCHBA_TLL_2020_test begin\n");

    PCHBA_TLL_2020 *test = new PCHBA_TLL_2020(&G1, &G2, &GT, &Zp);
    
    printf("——————————PG() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->PG(K, &skPCHBA, &pkPCHBA);
        te = std::chrono::high_resolution_clock::now();
        OutTime("PG", _, time_cast(te, ts));
    }
    printf("——————————PG() finished——————————\n");

    printf("——————————KG() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KG(&skPCHBA, &pkPCHBA, &attr_list, &ID, I, &sksPCHBA);
        te = std::chrono::high_resolution_clock::now();
        OutTime("keygen", _, time_cast(te, ts));
    }
    printf("——————————KG() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&pkPCHBA,&skPCHBA,&m, POLICY,&ID,J, &p, &h_, &b, &C, &c, &epk, &sigma);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));

        if(test->Check(&pkPCHBA, &m, &p, &h_, &b, &C, &c, &epk, &sigma)){
            printf("Check success\n");
        }
        else{
            printf("Check failed\n");
        }
    }
    printf("——————————Hash() finished——————————\n");

    printf("——————————Forge() start——————————\n");
    for(int _ = 0;_ < turns_f;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Forge(&pkPCHBA, &skPCHBA, &sksPCHBA, &m, &p, &h_, &b, &C, &c, &epk, &sigma, POLICY, &ID, I,&m_p, &p_p, &C_p, &c_p, &epk_p, &sigma_p);
        te = std::chrono::high_resolution_clock::now();
        OutTime("collision", _, time_cast(te, ts));

        if(test->Verify(&pkPCHBA, &m_p, &p_p, &h_, &b, &C_p, &c_p, &epk_p, &sigma_p)){
            printf("Verify success\n");
        }
        else{
            printf("Verify failed\n");
        }
    }
    printf("——————————Forge() finished——————————\n");

    printf("——————————Judge() start——————————\n");
    bool judgeRes;
    ts = std::chrono::high_resolution_clock::now();
    judgeRes = test->Judge(&pkPCHBA, &skPCHBA, &m, &p, &h_, &b, &C, &c, &epk, &sigma, &m_p, &p_p, &C_p, &c_p, &epk_p, &sigma_p, &ID, I);
    te = std::chrono::high_resolution_clock::now();
    OutTime("Judge", 0, time_cast(te, ts));

    if(judgeRes){
        printf("Judge success\n");
        test_result  = 0;
    }else{
        printf("Judge failed\n");
    } 
    
    printf("——————————Judge() finished——————————\n");

    if(out_file) fprintf(out, "-----------------------------------\n");
    if(visiable) printf("\n");
}

#undef time_cast

int main(int argc, char *argv[]) { // curve, scheme, turns, T;
    if(argc < 4) {
        printf("usage: %s [a|e|i|f|d224] {total_turns} [pg|kg|h|f|all]\n", argv[0]);
        return 0;
    }
    turns = atoi(argv[2]);

    if(strcmp(argv[3], "pg") == 0) turns_pg = turns;
    else if(strcmp(argv[3], "kg") == 0) turns_kg = turns;
    else if(strcmp(argv[3], "h") == 0) turns_h = turns;
    else if(strcmp(argv[3], "f") == 0) turns_f = turns;
    else if(strcmp(argv[3], "all") == 0) {
        turns_pg = turns;
        turns_kg = turns;
        turns_h = turns;
        turns_f = turns;
    }else return 0;
    
    if(strcmp(argv[1], "a") == 0) init_type(curves.a_param);
    // else if(strcmp(argv[1], "a80") == 0) init_type(curves.a_param_80);
    // else if(strcmp(argv[1], "a112") == 0) init_type(curves.a_param_112);
    // else if(strcmp(argv[1], "a128") == 0) init_type(curves.a_param_128);
    // else if(strcmp(argv[1], "a160") == 0) init_type(curves.a_param_160);
    else if(strcmp(argv[1], "a1") == 0) init_type(curves.a1_param);
    else if(strcmp(argv[1], "e") == 0) init_type(curves.e_param);
    else if(strcmp(argv[1], "i") == 0) init_type(curves.i_param);
    else if(strcmp(argv[1], "f") == 0) init_type(curves.f_param);
    else if(strcmp(argv[1], "d224") == 0) init_type(curves.d224_param);
    else return 0;
    out = fopen("tmp_PCHBA_TLL_2020.txt", "w");
    fflush(out);

    PCHBA_TLL_2020_test();

    fclose(out);
    return test_result;
}