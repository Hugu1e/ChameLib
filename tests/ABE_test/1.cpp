#include <ABE/ABET.h>
#include "curve/params.h"
#include <chrono>
#include <iostream>
#include <cstring>

int test_result = 1;

FILE *out = NULL;

int turns = 0, turns_pg = 1, turns_kg = 1, turns_h = 1, turns_f = 1;

const bool out_file = true, visiable = false;

pbc_param_t par;
pairing_t pairing;
element_t G1, G2, GT, Zp;





std::chrono::_V2::system_clock::time_point ts, te;

CurveParams curves;



void init_type(std::string &param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zp, pairing);


}


void OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    else if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

#define time_cast(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b).count()
    
void ABET_test() {
    printf("ABET_test begin\n");
    ABET *test = new ABET(&G1, &G2, &GT, &Zp);

    
    printf("——————————Setup() start——————————\n");
    for(int _ = 0;_ < turns_pg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Setup(&msk, &mpk, K);
        te = std::chrono::high_resolution_clock::now();
        OutTime("Setup", _, time_cast(te, ts));
    }
    printf("——————————Setup() finished——————————\n");


    printf("——————————KeyGen() start——————————\n");
    for(int _ = 0;_ < turns_kg;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->KeyGen(&msk, &mpk, &attr_list, &id, I, &sks);
        te = std::chrono::high_resolution_clock::now();
        OutTime("KeyGen", _, time_cast(te, ts));

        // PrintElement("sk0.sk0_1", sks.sk_0.sk0_1);
        // PrintElement("sk0.sk0_2", sks.sk_0.sk0_2);
        // PrintElement("sk0.sk0_3", sks.sk_0.sk0_3);
        // PrintElement("sk0.sk0_4", sks.sk_0.sk0_4);
        // PrintElement("sk0.sk0_5", sks.sk_0.sk0_5);
        // PrintElement("sk0.sk0_6", sks.sk_0.sk0_6);
        // for(int i = 0;i < SIZE_OF_ATTR;i++){
        //     PrintElement("sk_y[" + std::to_string(i) + "].sk_1", sks.sk_y[i]->sk_1);
        //     PrintElement("sk_y[" + std::to_string(i) + "].sk_2", sks.sk_y[i]->sk_2);
        //     PrintElement("sk_y[" + std::to_string(i) + "].sk_3", sks.sk_y[i]->sk_3);
        // }
        // PrintElement("sk_prime.sk_1", sks.sk_prime.sk_1);
        // PrintElement("sk_prime.sk_2", sks.sk_prime.sk_2);
        // PrintElement("sk_prime.sk_3", sks.sk_prime.sk_3);
        // PrintElement("sk1", sks.sk1);
        // for(int i = 1;i <= I-1;i++){
        //     PrintElement("sk2[" + std::to_string(i) + "]", *sks.sk2[i]);
        // }
    }
    printf("——————————KeyGen() finished——————————\n");

    printf("——————————Encrypt() start——————————\n");
    for(int _ = 0;_ < turns_h;_++) {
        element_random(r);
        element_random(R);
        PrintElement("R", R);
        PrintElement("r", r);

        element_random(s1);
        element_random(s2);

        ts = std::chrono::high_resolution_clock::now();
        test->Encrypt(&mpk, &msk, &r, &R, POLICY, &id, J, &s1, &s2, &ciphertext);
        te = std::chrono::high_resolution_clock::now();
        OutTime("Encrypt", _, time_cast(te, ts));

        // PrintElement("ct0.ct_1", ciphertext.ct_0.ct0_1);
        // PrintElement("ct0.ct_2", ciphertext.ct_0.ct0_2);
        // PrintElement("ct0.ct_3", ciphertext.ct_0.ct0_3);
        // PrintElement("ct0.ct_4", ciphertext.ct_0.ct0_4);
        // for(int i = 0;i < SIZE_OF_POLICY;i++){
        //     PrintElement("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.ct_y[i]->ct_1);
        //     PrintElement("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.ct_y[i]->ct_2);
        //     PrintElement("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.ct_y[i]->ct_3);
        // }
        // PrintElement("ct_", ciphertext.ct_);
        // PrintElement("ct_prime", ciphertext.ct_prime);
        // PrintElement("ct1", ciphertext.ct1);
        // PrintElement("ct2", ciphertext.ct2);
        // PrintElement("ct3", ciphertext.ct3);
    }
    printf("——————————Encrypt() finished——————————\n");

    printf("——————————Decrypt() start——————————\n");
    for(int _ = 0;_ < turns_f;_++) {
        ts = std::chrono::high_resolution_clock::now();
        test->Decrypt(&mpk, &ciphertext, &sks, &res_R, &res_r);
        te = std::chrono::high_resolution_clock::now();
        OutTime("Decrypt", _, time_cast(te, ts));

        PrintElement("res_R", res_R);
        PrintElement("res_r", res_r);

        if(element_cmp(R,res_R) == 0  && element_cmp(r,res_r) == 0){
            printf("Decrypt successfully.\n");
            test_result = 0;
        }else{
            printf("Decrypt failed.\n");
        } 
    }
    printf("——————————Decrypt() finished——————————\n");

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
    out = fopen("tmp_ABET.txt", "w");
    fflush(out);

    ABET_test();

    fclose(out);

    return test_result;
}