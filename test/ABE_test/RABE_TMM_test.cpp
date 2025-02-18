#include "ABE/RABE_TMM.h"
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    RABE_TMM abe(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const int N = 8;  // a binary tree with N leaf nodes
    const int DEPTH = 4;  // log2(N) + 1
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

    element_t id1, id2, id3;
    element_init_same_as(id1, test.get_Zn());
    element_init_same_as(id2, test.get_Zn());
    element_init_same_as(id3, test.get_Zn());
    element_random(id1);
    element_random(id2);
    element_random(id3);

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);

    RABE_TMM_mpk mpk;
    RABE_TMM_msk msk;
    RABE_TMM_skid skid1, skid2, skid3;
    RABE_TMM_kut kut;
    RABE_TMM_dkidt dkidt1, dkidt2, dkidt3;
    RABE_TMM_ciphertext ciphertext;

    std::vector<RABE_TMM_revokedPreson> rl;
    Binary_tree_RABE st;

    element_t msg,res;
    element_t s1,s2;

    element_init_same_as(msg, test.get_Zn());
    element_init_same_as(res, test.get_Zn());
    element_init_same_as(s1, test.get_Zn());
    element_init_same_as(s2, test.get_Zn());

    element_random(s1);
    element_random(s2);

    test.start("Setup");
    abe.Setup(mpk, msk, rl, st, N);
    test.end("Setup");
    // st->printTree();
    // Logger::PrintPbc("g", msk.getElement("g"));
    // Logger::PrintPbc("h", msk.getElement("h"));
    // Logger::PrintPbc("a1", msk.getElement("a1"));
    // Logger::PrintPbc("a2", msk.getElement("a2"));
    // Logger::PrintPbc("b1", msk.getElement("b1"));
    // Logger::PrintPbc("b2", msk.getElement("b2"));
    // Logger::PrintPbc("g_pow_d1", msk.getElement("g_pow_d1"));
    // Logger::PrintPbc("g_pow_d2", msk.getElement("g_pow_d2"));
    // Logger::PrintPbc("g_pow_d3", msk.getElement("g_pow_d3"));
    // Logger::PrintPbc("h", mpk.getElement("h"));
    // Logger::PrintPbc("H1", mpk.getElement("H1"));
    // Logger::PrintPbc("H2", mpk.getElement("H2"));
    // Logger::PrintPbc("T1", mpk.getElement("T1"));
    // Logger::PrintPbc("T2", mpk.getElement("T2"));

    test.start("KeyGen");
    abe.KGen(skid1, st, mpk, msk, attr_list, id1, re_time_1);
    test.end("KeyGen");
    abe.KGen(skid2, st, mpk, msk, attr_list, id2, re_time_2);
    abe.KGen(skid3, st, mpk, msk, attr_list, id3, re_time_3);
    // Logger::PrintPbc("sk0.sk_1", skid.get_sk0()->getElement("sk0_1"));
    // Logger::PrintPbc("sk0.sk_2", skid.get_sk0()->getElement("sk0_2"));
    // Logger::PrintPbc("sk0.sk_3", skid.get_sk0()->getElement("sk0_3"));
    // for(int i = 0;i < SIZE_OF_ATTR;i++){
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_1", skid.get_sk_y(i)->getElement("sk_1"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_2", skid.get_sk_y(i)->getElement("sk_2"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_3", skid.get_sk_y(i)->getElement("sk_3"));
    // }
    // Logger::PrintPbc("sk_prime.sk_1", skid.get_sk_prime()->getElement("sk_1"));
    // Logger::PrintPbc("sk_prime.sk_2", skid.get_sk_prime()->getElement("sk_2"));
    // Logger::PrintPbc("sk_prime.sk_3", skid.get_sk_prime()->getElement("sk_3"));


    test.start("Rev");
    abe.Rev(rl, id1, re_time_1);
    test.end("Rev");
    abe.Rev(rl, id2, re_time_2);
    abe.Rev(rl, id3, re_time_3);


    element_random(msg);
    Logger::PrintPbc("msg", msg);
    test.start("Encrypt");
    abe.Enc(ciphertext, mpk, msg, POLICY, T_present, s1, s2);
    test.end("Encrypt");
    // Logger::PrintPbc("ct0.ct_1", ciphertext.get_ct0()->getElement("ct0_1"));
    // Logger::PrintPbc("ct0.ct_2", ciphertext.get_ct0()->getElement("ct0_2"));
    // Logger::PrintPbc("ct0.ct_3", ciphertext.get_ct0()->getElement("ct0_3"));
    // for(int i = 0;i < SIZE_OF_POLICY;i++){
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.get_ct_y(i)->getElement("ct_1"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.get_ct_y(i)->getElement("ct_2"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.get_ct_y(i)->getElement("ct_3"));
    // }
    // Logger::PrintPbc("ct_prime", ciphertext.get_ct_prime()->getElement("ct_prime"));




    test.start("KUpt");
    abe.KUpt(kut, mpk, st, rl, T_present);
    test.end("KUpt");
    printf("size of kut.ku_theta: %ld\n", kut.get_ku_theta().size());

    test.start("DKGen");
    try{
        abe.DKGen(dkidt1, mpk, skid1, kut);
    }catch(const std::runtime_error& e){
        printf("%s\n", e.what());
    }
    test.end("DKGen");

    test.start("DKGen");
    try{
        abe.DKGen(dkidt3, mpk, skid3, kut);
    }catch(const std::runtime_error& e){
        printf("%s\n", e.what());
    }
    test.end("DKGen");

    test.start("DKGen");
    abe.DKGen(dkidt2, mpk, skid2, kut);
    test.end("DKGen");

   
    test.start("Decrypt");
    abe.Dec(res, mpk, ciphertext, dkidt2);
    test.end("Decrypt");

    Logger::PrintPbc("msg", msg);
    Logger::PrintPbc("res", res);

    if(element_cmp(msg, res) == 0){
        printf("Decrypt successfully.\n");
        test_result = 0;
    }else{
        printf("Decrypt failed.\n");
    } 

}


int main(int argc, char *argv[]){
    if(argc == 1) {
        test(argv[0], "a");
    }else if(argc == 2){
        test(argv[0], argv[1]);
    }else{
        printf("usage: %s [a|e|i|f|d224]\n", argv[0]);
        return 1;
    }
    return test_result;
}

