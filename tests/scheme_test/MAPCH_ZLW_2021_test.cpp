#include <scheme/MAPCH_ZLW_2021.h>
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


const string POLICY = "(ONE&THREE)&(TWO|FOUR)";
const int SIZE_OF_POLICY = 4;

// attribute
const string A = "ONE";
const string B = "TWO";
const string C = "THREE";
const string D = "FOUR";
vector<string> As = {A, B, C, D};
const int SIZE_OF_ATTRIBUTES = As.size();
const string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";


const int K = 256;
MAPCH_ZLW_2021::pp pp;
vector<MAPCH_ZLW_2021::mhk *> mhks;
vector<MAPCH_ZLW_2021::mtk *> mtks;
vector<MAPCH_ZLW_2021::mski *> mskis;
MAPCH_ZLW_2021::h h,h_p;

string m = "message";
string m_p = "message_p";


std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;

void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);

    h.Init(&G1, &GT, SIZE_OF_POLICY);
    h_p.Init(&G1, &GT, SIZE_OF_POLICY);
}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void MAPCH_ZLW_2021_test() {
    printf("MAPCH_ZLW_2021_test begin\n");
    MAPCH_ZLW_2021 *test = new MAPCH_ZLW_2021(&G1, &G2, &Zp, &GT);

    printf("k = %d\n", K);
    
    printf("——————————SetUp() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->SetUp(&pp, &mhks, &mtks, K, &As);
        te = std::chrono::high_resolution_clock::now();
        OutTime("SetUp", _, time_cast(te, ts));
    }
    printf("——————————SetUp() finished——————————\n");

    printf("——————————KeyGen() start——————————\n");
    for(int _ = 0;_ < 1;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KeyGen(&mskis, &mtks, &mhks, &As, GID);
        te = std::chrono::high_resolution_clock::now();
        OutTime("KeyGen", _, time_cast(te, ts));
    }
    printf("——————————KeyGen() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&h, &pp, &mhks, m, POLICY);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));

        PrintMpz("h0", h.h.h0);
        PrintMpz("h1", h.h.h1);
        PrintMpz("r0", h.r.r0);
        PrintMpz("r1", h.r.r1);

        
        if(test->Check(&mhks, m, &h)){
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
        
        // test->Forge(&h_p, &mhks, &mskis, m, m_p, &h);
        mskis.pop_back();
        test->Forge(&h_p, &mhks, &mskis, m, m_p, &h);

        te = std::chrono::high_resolution_clock::now();

        PrintMpz("h0'", h_p.h.h0);
        PrintMpz("h1'", h_p.h.h1);
        PrintMpz("r0'", h_p.r.r0);
        PrintMpz("r1'", h_p.r.r1);
       
        if(test->Verify(&mhks, m_p, &h_p)){
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
    out = fopen("tmp_MAPCH_ZLW_2021.txt", "w");
    fflush(out);

    MAPCH_ZLW_2021_test();

    fclose(out);
    return test_result;
}