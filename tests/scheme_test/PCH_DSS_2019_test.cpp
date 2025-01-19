#include <scheme/PCH_DSS_2019.h>
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

std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
const int SIZE_OF_ATTR = attr_list.size();  // S
const string POLICY = "(ONE&THREE)&(TWO|FOUR)";
const int SIZE_OF_POLICY = 4;

int k;
PCH_DSS_2019::skPCH skPCH;
PCH_DSS_2019::pkPCH pkPCH;
PCH_DSS_2019::sksPCH sksPCH;

mpz_t m,m_p;
PCH_DSS_2019::h h;
PCH_DSS_2019::r r,r_p;

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

    skPCH.Init(&G1, &G2, &Zp);
    pkPCH.Init(&G2, &GT);
    sksPCH.Init(&G1, &G2, SIZE_OF_ATTR);
    h.Init(&G1, &G2, &GT, SIZE_OF_POLICY);
    r.Init();
    r_p.Init();
    
    

}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void PCH_DSS_2019_test() {
    printf("PCH_DSS_2019_test begin\n");

    k = 512;
    printf("k = %d\n", k);
    GenerateRandomWithLength(m, 128);
    PrintMpzAndSize("m", m);
    GenerateRandomWithLength(m_p, 128);
    PrintMpzAndSize("m_p", m_p);

    PCH_DSS_2019 *test = new PCH_DSS_2019(&n,&e,&d,&G1, &G2, &Zp, &GT);
    
    printf("——————————PG() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->PG(k, &skPCH, &pkPCH);
        te = std::chrono::high_resolution_clock::now();
        OutTime("PG", _, time_cast(te, ts));

    }
    printf("——————————PG() finished——————————\n");

    printf("——————————KG() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KG(&skPCH, &pkPCH, &attr_list, &sksPCH);
        te = std::chrono::high_resolution_clock::now();
        OutTime("keygen", _, time_cast(te, ts));

    }
    printf("——————————KG() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&pkPCH, &m, POLICY, &h, &r);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));

        PrintMpzAndSize("h1", h.h1);
        PrintMpzAndSize("h2", h.h2);
        PrintMpzAndSize("N2", h.N2);
        
        if(test->Check(&pkPCH, &m, &h, &r)){
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
        test->Forge(&pkPCH, &sksPCH, &m, &m_p, &h, &r, &r_p);
        te = std::chrono::high_resolution_clock::now();
       
        if(test->Verify(&pkPCH, &m_p, &h, &r_p)){
            printf("Verify success\n");
            test_result = 0;
        }
        else{
            printf("Verify failed\n");
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
    out = fopen("tmp_PCH_DSS_2019.txt", "w");
    fflush(out);

    PCH_DSS_2019_test();

    fclose(out);
    return test_result;
}