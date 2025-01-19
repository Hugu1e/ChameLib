#include <scheme/CH_KEF_NoMH_AM_2004.h>
#include "curve/params.h"
#include <chrono>
#include <iostream>
#include <cstring>
#include "pbc/pbc.h"

int test_result = 1;

FILE *out = NULL;
int turns = 0, turns_pg = 1, turns_kg = 1, turns_h = 1, turns_f = 1;
const bool out_file = true, visiable = false;
std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;
pbc_param_t par;
pairing_t pairing;
element_t G1, G2, GT, Zp;


CH_KEF_NoMH_AM_2004::pk pk;
CH_KEF_NoMH_AM_2004::sk sk;
element_t m,m_p;
element_t r,r_p,s,s_p;
element_t h;


void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);

    pk.Init(&Zp);
    sk.Init(&Zp);
    element_init_same_as(m, Zp);
    element_init_same_as(m_p, Zp);
    element_init_same_as(r, Zp);
    element_init_same_as(r_p, Zp);
    element_init_same_as(s, Zp);
    element_init_same_as(s_p, Zp);
    element_init_same_as(h, Zp);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void CH_KEF_NoMH_AM_2004_test() {
    printf("CH_KEF_NoMH_AM_2004_test begin\n");
    CH_KEF_NoMH_AM_2004 *test = new CH_KEF_NoMH_AM_2004(&G1, &G2, &GT, &Zp);

    element_random(m);
    element_random(m_p);
    
    printf("——————————KG() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KeyGen(&pk, &sk);
        te = std::chrono::high_resolution_clock::now();
        OutTime("keygen", _, time_cast(te, ts));
    }
    printf("——————————KG() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&pk, &m, &r, &s, &h);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));

        PrintElementAndSize("r", r);
        PrintElementAndSize("s", s);
        PrintElementAndSize("h", h);

        if(test->Check(&pk, &m, &r, &s, &h)){
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
        test->Forge(&pk, &sk, &m_p, &h, &r_p, &s_p);
        te = std::chrono::high_resolution_clock::now();
        OutTime("collision", _, time_cast(te, ts));

        if(test->Verify(&pk, &m_p, &r_p, &s_p, &h)){
            printf("Verify success\n");
            test_result = 0;
        }
        else{
            printf("Verify failed\n");
        }
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
    out = fopen("tmp_CH_KEF_NoMH_AM_2004.txt", "w");
    fflush(out);

    CH_KEF_NoMH_AM_2004_test();

    fclose(out);
    return test_result;
}