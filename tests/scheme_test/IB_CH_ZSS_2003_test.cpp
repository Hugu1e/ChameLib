#include <scheme/IB_CH_ZSS_2003.h>
#include "curve/params.h"
#include <chrono>
#include <iostream>
#include <cstring>
#include "pbc/pbc.h"

int test_result = 1;

FILE *out = NULL;

int turns = 0, turns_setup = 1, turns_extract = 1, turns_hash = 1, turns_forge = 1, rev_G1G2;

const bool out_file = true, visiable = false;

pbc_param_t par;
pairing_t pairing;
element_t G1, G2, GT, Zp, S_ID, ID, m, m_p, R, R_p, H;

std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;

void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);
    if(rev_G1G2) {
        element_init_G2(G1, pairing);
        element_init_G1(G2, pairing);
    } else {
        element_init_G1(G1, pairing);
        element_init_G2(G2, pairing);
    }
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);
    
    element_init_same_as(S_ID, G1);
    element_init_same_as(R, G1);
    element_init_same_as(R_p, G1);
    element_init_same_as(H, GT);
    element_init_same_as(ID, Zp);
    element_init_same_as(m, Zp);
    element_init_same_as(m_p, Zp);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void IB_CH_S1_test() {
    IB_CH_S1 *test = new IB_CH_S1(&G1, &G2, &Zp, &GT, rev_G1G2);

    if(out_file) fprintf(out, "public_key: %d B, secret_key: %d B, hash_value: %d B, random_value: %d B\n", test->public_key_size(), CountSize(S_ID), CountSize(H), CountSize(R));
    if(out_file) fprintf(out, "turns_setup: %d, turns_extract: %d, turns_hash: %d, turns_forge: %d\n", turns_setup, turns_extract, turns_hash, turns_forge);

    for(int _ = 0;_ < turns_setup;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Setup();
        te = std::chrono::high_resolution_clock::now();
        OutTime("setup", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_extract;_++) {
        element_random(ID);
        ts = std::chrono::high_resolution_clock::now();
        test->Extract(&S_ID, &ID);
        te = std::chrono::high_resolution_clock::now();
        OutTime("keygen", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_hash;_++) {
        element_random(m);
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&H, &R, &ID, &m);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_forge;_++) {
        element_random(m_p);
        ts = std::chrono::high_resolution_clock::now();
        test->Forge(&ID, &S_ID, &m, &m_p, &R, &R_p);
        te = std::chrono::high_resolution_clock::now();
        OutTime("collision", _, time_cast(te, ts));

        if(test->Verify(&H, &R_p, &ID, &m_p)){
            printf("Verify success.\n");
            test_result = 0;
        }else{
            printf("Verify failed.\n");
        }
    }

    if(out_file) fprintf(out, "-----------------------------------\n");
    if(visiable) printf("\n");
}

void IB_CH_S2_test() {
    IB_CH_S2 *test = new IB_CH_S2(&G1, &G2, &Zp, &GT, rev_G1G2);

    if(out_file) fprintf(out, "public_key: %d B, secret_key: %d B, hash_value: %d B, random_value: %d B\n", test->public_key_size(), CountSize(S_ID), CountSize(H), CountSize(R));
    if(out_file) fprintf(out, "turns_setup: %d, turns_extract: %d, turns_hash: %d, turns_forge: %d\n", turns_setup, turns_extract, turns_hash, turns_forge);

    for(int _ = 0;_ < turns_setup;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Setup();
        te = std::chrono::high_resolution_clock::now();
        OutTime("setup", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_extract;_++) {
        element_random(ID);
        ts = std::chrono::high_resolution_clock::now();
        test->Extract(&S_ID, &ID);
        te = std::chrono::high_resolution_clock::now();
        OutTime("keygen", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_hash;_++) {
        element_random(m);
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&H, &R, &ID, &m);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));
    }

    for(int _ = 0;_ < turns_forge;_++) {
        element_random(m_p);
        ts = std::chrono::high_resolution_clock::now();
        test->Forge(&ID, &S_ID, &m, &m_p, &R, &R_p);
        te = std::chrono::high_resolution_clock::now();
        OutTime("collision", _, time_cast(te, ts));

        if(test->Verify(&H, &R_p, &ID, &m_p)){
            printf("Verify success.\n");
            test_result = 0;
        }else{
            printf("Verify failed.\n");
        }
    }

    if(out_file) fprintf(out, "-----------------------------------\n");
    if(visiable) printf("\n");
}
    

#undef time_cast

int main(int argc, char *argv[]) { // curve, scheme, turns, T;
    if(argc < 5) {
        printf("usage: %s [a|e|i|f|d224] {total_turns} [setup|hash|keygen|collision|all] [0|1] [S1|S2]\n", argv[0]);
        return 0;
    }
    turns = atoi(argv[2]);
    rev_G1G2 = atoi(argv[4]);

    if(strcmp(argv[3], "setup") == 0) turns_setup = turns;
    else if(strcmp(argv[3], "hash") == 0) turns_hash = turns;
    else if(strcmp(argv[3], "keygen") == 0) turns_extract = turns;
    else if(strcmp(argv[3], "collision") == 0) turns_forge = turns;
    else if(strcmp(argv[3], "all") == 0) {
        turns_setup = turns;
        turns_hash = turns;
        turns_extract = turns;
        turns_forge = turns;
    }else return 0;
    
    if(strcmp(argv[1], "a") == 0) init_type(curves.a_param);
    // else if(strcmp(argv[1], "a80") == 0) init_type(curves.a_param_80);
    // else if(strcmp(argv[1], "a112") == 0) init_type(curves.a_param_112);
    // else if(strcmp(argv[1], "a128") == 0) init_type(curves.a_param_128);
    // else if(strcmp(argv[1], "a160") == 0) init_type(curves.a_param_160);
    // else if(strcmp(argv[1], "a1") == 0) init_type(curves.a1_param);
    else if(strcmp(argv[1], "e") == 0) init_type(curves.e_param);
    else if(strcmp(argv[1], "i") == 0) init_type(curves.i_param);
    else if(strcmp(argv[1], "f") == 0) init_type(curves.f_param);
    else if(strcmp(argv[1], "d224") == 0) init_type(curves.d224_param);
    else return 0;
    out = fopen("tmp.txt", "w");
    fflush(out);
    if(strcmp(argv[5], "S1") == 0) IB_CH_S1_test();
    else if(strcmp(argv[5], "S2") == 0) IB_CH_S2_test();

    fclose(out);
    return test_result;
}