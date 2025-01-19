#include <scheme/CH_KEF_DLP_LLA_2012.h>
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

CH_KEF_DLP_LLA_2012::sk sk;
CH_KEF_DLP_LLA_2012::pk pk;
CH_KEF_DLP_LLA_2012::label label;
    
element_t m, m_p,m_pp;
element_t r, r_p, r_pp;
element_t S;  // hash value

std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;

void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);

    sk.Init(&Zp);
    pk.Init(&G1);
    label.Init(&G1);

    element_init_same_as(m, Zp);
    element_init_same_as(m_p, Zp);
    element_init_same_as(m_pp, Zp);
    element_init_same_as(r, Zp);
    element_init_same_as(r_p, Zp);
    element_init_same_as(r_pp, Zp);
    element_init_same_as(S, G1);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void CH_KEF_DLP_LLA_2012_test() {
    printf("CH_KEF_DLP_LLA_2012_test begin\n");

    element_random(m);
    element_random(m_p);
    element_random(m_pp);
    PrintElementAndSize("m", m);
    PrintElementAndSize("m_p", m_p);
    PrintElementAndSize("m_pp", m_pp);
    

    CH_KEF_DLP_LLA_2012 *test = new CH_KEF_DLP_LLA_2012(&G1, &G2, &Zp, &GT);
    
    printf("——————————PG() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->PG();
        te = std::chrono::high_resolution_clock::now();
        OutTime("PG", _, time_cast(te, ts));
    }
    printf("——————————PG() finished——————————\n");

    printf("——————————KG() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KG(&sk, &pk, &label);
        te = std::chrono::high_resolution_clock::now();

        PrintElementsize("sk.a", sk.a);
        PrintElementsize("sk.x1", sk.x1);
        PrintElementsize("sk.x2", sk.x2);
        
        OutTime("keygen", _, time_cast(te, ts));
    }
    printf("——————————KG() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&pk, &m, &r, &label, &S);
        te = std::chrono::high_resolution_clock::now();

        PrintElement("r", r);
        PrintElementAndSize("Hash value", S);

        OutTime("hash", _, time_cast(te, ts));
    }
    printf("——————————Hash() finished——————————\n");

    printf("——————————Forge() start——————————\n");
    for(int _ = 0;_ < turns_f;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->UForge(&sk, &pk, &label, &S, &m, &m_p, &r, &r_p);
        test->IForge(&label, &m, &m_p, &r, &r_p, &m_pp, &r_pp);
        te = std::chrono::high_resolution_clock::now();
        
        if(test->Verify(&m_p, &r_p, &pk, &label, &S)){
            printf("Uforge Verify success\n");
        }
        else{
            printf("Uforge Verify failed\n");
        }
        if(test->Verify(&m_pp, &r_pp, &pk, &label, &S)){
            printf("Iforge Verify success\n");
            test_result = 0;
        }
        else{
            printf("Iforge Verify failed\n");
        }
        
        OutTime("collision", _, time_cast(te, ts));
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
    out = fopen("tmp_CH_KEF_DLP_LLA_2012.txt", "w");
    fflush(out);

    CH_KEF_DLP_LLA_2012_test();

    fclose(out);
    return test_result;
}