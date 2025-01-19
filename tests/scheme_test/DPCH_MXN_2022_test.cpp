#include <scheme/DPCH_MXN_2022.h>
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
const string ATTRIBUTES[] = {A, B, C, D};
const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);
const string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

vector<DPCH_MXN_2022::pkTheta *> pkThetas;
vector<DPCH_MXN_2022::skTheta *> skThetas;
vector<DPCH_MXN_2022::skGidA *> skGidAs;

int k;
DPCH_MXN_2022::pp ppDPCH;
DPCH_MXN_2022::pkDPCH pkDPCH;
DPCH_MXN_2022::skDPCH skDPCH;
DPCH_MXN_2022::skGid skGid;
DPCH_MXN_2022::sigmaGid sigmaGid;

DPCH_MXN_2022::h h;
DPCH_MXN_2022::r r,r_p;
DPCH_MXN_2022::c c;

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


    ppDPCH.Init(&G1);
    pkDPCH.Init(&G1);
    skDPCH.Init(&Zp);
    sigmaGid.Init(&G2);
    
    c.Init(&G1, &GT, SIZE_OF_POLICY);

    
    

}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void DPCH_MXN_2022_test() {
    printf("DPCH_MXN_2022_test begin\n");

    k = 1024;
    printf("k = %d\n", k);
    // GenerateRandomWithLength(m, 128);
    // PrintMpzAndSize("m", m);
    // GenerateRandomWithLength(m_p, 128);
    // PrintMpzAndSize("m_p", m_p);

    DPCH_MXN_2022 *test = new DPCH_MXN_2022(&G1, &G2, &Zp, &GT);
    
    printf("——————————SetUp() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->SetUp(&ppDPCH, &pkDPCH, &skDPCH, k);
        te = std::chrono::high_resolution_clock::now();
        OutTime("SetUp", _, time_cast(te, ts));

        // PrintMpz("n0", pkDPCH.pkCHET.n0);
        // PrintMpz("e0", pkDPCH.pkCHET.e0);
        // PrintMpz("d0", skDPCH.skCHET.d0);
        // PrintElement("g", ppDPCH.g);
        // PrintElement("y", pkDPCH.pkBLS.y);
        // PrintElement("a", skDPCH.skBLS.a);
    }
    printf("——————————SetUp() finished——————————\n");

    printf("——————————ModSetUp() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->ModSetUp(&skGid, &sigmaGid, &skDPCH, GID);
        te = std::chrono::high_resolution_clock::now();
        OutTime("ModSetUp", _, time_cast(te, ts));

        PrintMpz("skGid.skCH.d0", skGid.skCH.d0);
        PrintElement("signature", sigmaGid.signature.sigma);
    }
    printf("——————————ModSetUp() finished——————————\n");

    printf("——————————AuthSetUp() start——————————\n");
    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        DPCH_MXN_2022::pkTheta *pkTheta = new DPCH_MXN_2022::pkTheta; 
        DPCH_MXN_2022::skTheta *skTheta = new DPCH_MXN_2022::skTheta;
        pkTheta->Init(&G1, &GT);
        skTheta->Init(&Zp);

        ts = std::chrono::high_resolution_clock::now();
        test->AuthSetUp(pkTheta, skTheta, &ppDPCH, ATTRIBUTES[_]);
        te = std::chrono::high_resolution_clock::now();
        OutTime("AuthSetUp", _, time_cast(te, ts));

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);

        // PrintElement("pkTheta", pkTheta->pk.pkTheta_1);
        // PrintElement("pkTheta", pkTheta->pk.pkTheta_2);
        // PrintElement("skTheta", skTheta->sk.aTheta);
        // PrintElement("skTheta", skTheta->sk.yTheta);
    }
    printf("——————————AuthSetUp() finished——————————\n");

    printf("——————————ModKeyGen() start——————————\n");
    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        DPCH_MXN_2022::skGidA *skGidA = new DPCH_MXN_2022::skGidA;
        skGidA->Init(&G1);

        ts = std::chrono::high_resolution_clock::now();
        test->ModKeyGen(skGidA, &ppDPCH, &pkDPCH, GID, &sigmaGid, skThetas[_], ATTRIBUTES[_]);
        te = std::chrono::high_resolution_clock::now();
        OutTime("ModKeyGen", _, time_cast(te, ts));

        skGidAs.push_back(skGidA);
        
        // printf("skGidA->gid: %s\n", skGidA->sk.gid.c_str());
        // printf("skGidA->A: %s\n", skGidA->sk.A.c_str());
        // PrintElement("skgidA_0", skGidA->sk.skgidA_0);
        // PrintElement("skgidA_1", skGidA->sk.skgidA_1);
    }
    printf("——————————ModKeyGen() finished——————————\n");

    printf("——————————Hash() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Hash(&h, &r, &c, &ppDPCH, &pkDPCH, m, &pkThetas, POLICY);
        te = std::chrono::high_resolution_clock::now();
        OutTime("hash", _, time_cast(te, ts));

        PrintMpz("h0", h.h.h0);
        PrintMpz("h1", h.h.h1);
        PrintMpz("r0", r.r.r0);
        PrintMpz("r1", r.r.r1);

        
        if(test->Check(&pkDPCH, m, &h, &r)){
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
        test->Forge(&r_p, &pkDPCH, &skGid, &skGidAs, &c, m, m_p, &h, &r);
        te = std::chrono::high_resolution_clock::now();

        PrintMpz("r0'", r_p.r.r0);
        PrintMpz("r1'", r_p.r.r1);
       
        if(test->Verify(&pkDPCH, m_p, &h, &r_p)){
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
    out = fopen("tmp_DPCH_MXN_2022.txt", "w");
    fflush(out);

    DPCH_MXN_2022_test();

    fclose(out);
    return test_result;
}