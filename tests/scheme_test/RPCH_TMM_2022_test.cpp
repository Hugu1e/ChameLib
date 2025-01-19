#include <scheme/RPCH_TMM_2022.h>
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
mpz_t n,e,d;

const int N = 8;  // a binary tree with N leaf nodes
std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
const int SIZE_OF_ATTR = attr_list.size();  // S
const string POLICY = "(ONE&THREE)&(TWO|FOUR)";
const int SIZE_OF_POLICY = 4;
const time_t T = TimeCast(2024, 12, 21, 0, 0, 0);  // present time


int k;
RPCH_TMM_2022::skRPCH skRPCH;
RPCH_TMM_2022::pkRPCH pkRPCH;
RPCH_TMM_2022::skidRPCH skidRPCH;
RPCH_TMM_2022::dkidtRPCH dkidtRPCH;
RABE_TMM::kut kut;


vector<RABE_TMM::revokedPreson *> rl;
binary_tree_RABE *st;
element_t id;

RABE_TMM::ciphertext C;
element_t m,m_p;
element_t b;  // hash value
element_t h;
element_t r,r_p;

std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;

void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);

    mpz_inits(n,e,d,m,m_p,NULL);

    skRPCH.Init(&G1, &G2, &Zp);
    pkRPCH.Init(&G1, &G2, &GT);
    skidRPCH.Init(&G1, &G2, &Zp, SIZE_OF_ATTR);
    dkidtRPCH.Init(&G1, &G2, &Zp, SIZE_OF_ATTR);
    C.Init(&G1, &G2, &Zp, SIZE_OF_POLICY);

    
    element_init_same_as(id, Zp);
    element_init_same_as(b, G1);
    element_init_same_as(h, G1);
    element_init_same_as(r, Zp);
    element_init_same_as(r_p, Zp);
    element_init_same_as(m, Zp);
    element_init_same_as(m_p, Zp);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void RPCH_TMM_2022_test() {
    printf("RPCH_TMM_2022_test begin\n");

    k = 512;
    printf("k = %d\n", k);
    element_random(m);
    element_random(m_p);

    RPCH_TMM_2022 *test = new RPCH_TMM_2022(&n,&e,&d,&G1, &G2, &Zp, &GT);
    
    printf("——————————PG() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->PG(k, N, &skRPCH, &pkRPCH, &rl, st);
        te = std::chrono::high_resolution_clock::now();
        OutTime("PG", _, time_cast(te, ts));

    }
    printf("——————————PG() finished——————————\n");

    printf("——————————KG() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KG(&pkRPCH, &skRPCH, st, &id, &attr_list, &skidRPCH);
        te = std::chrono::high_resolution_clock::now();
        OutTime("KG", _, time_cast(te, ts));

    }
    printf("——————————KG() finished——————————\n");

    printf("——————————KUpt() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KUpt(&pkRPCH, st, &rl, T, &kut);
        te = std::chrono::high_resolution_clock::now();
        OutTime("KUpt", _, time_cast(te, ts));

        printf("size of kut.ku_theta: %ld\n", kut.ku_theta.size());
    }
    printf("——————————KUpt() finished——————————\n");

    printf("——————————DKGen() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->DKGen(&pkRPCH, &skidRPCH, &kut, &dkidtRPCH);
        te = std::chrono::high_resolution_clock::now();
        OutTime("DKGen", _, time_cast(te, ts));

        printf("size of kut.ku_theta: %ld\n", kut.ku_theta.size());
    }
    printf("——————————DKGen() finished——————————\n");

    printf("——————————Rev() start——————————\n");
    for(int _ = 0;_ < turns_f;_++) {
        ts = std::chrono::high_resolution_clock::now();
        time_t target_time = TimeCast(2025, 12, 31, 0, 0, 0);
        test->Rev(&rl, &id, target_time);
        te = std::chrono::high_resolution_clock::now();
        OutTime("Rev", _, time_cast(te, ts));
    }
    printf("——————————Rev() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&pkRPCH, &m, POLICY, T, &b, &r, &h, &C);
        te = std::chrono::high_resolution_clock::now();
        OutTime("Hash", _, time_cast(te, ts));

        
        if(test->Check(&pkRPCH, &m, &b, &r, &h)){
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
        test->Forge(&pkRPCH, &dkidtRPCH, &m, &m_p, &b, &r, &h, &C, &r_p);
        te = std::chrono::high_resolution_clock::now();
       
        if(test->Verify(&pkRPCH, &m_p, &b, &r_p, &h)){
            printf("Verify success\n");
            test_result = 0;
        }
        else{
            printf("Verify failed\n");
        }
        OutTime("Forge", _, time_cast(te, ts));
    }
    printf("——————————Forge() finished——————————\n");

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
    out = fopen("tmp_RPCH_TMM_2022.txt", "w");
    fflush(out);

    RPCH_TMM_2022_test();

    fclose(out);
    return test_result;
}