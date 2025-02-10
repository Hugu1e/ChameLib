#include <ABE/RABE_XNM.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    RABE_XNM abe(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const int N = 8;  // a binary tree with N leaf nodes
    const int DEPTH = 4;  // log2(N) + 1
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）
    const time_t T = TimeUtils::TimeCast(2024, 12, 21, 0, 0, 0);  // present time

    RABE_XNM_mpk mpk;
    RABE_XNM_msk msk;
    RABE_XNM_skid skid;
    RABE_XNM_kut kut;
    RABE_XNM_dkidt dkidt;
    RABE_XNM_ciphertext ciphertext;

    std::vector<RABE_XNM_revokedPreson> rl;
    Binary_tree_RABE st;

    element_t id;
    element_t msg,res;
    element_t s1,s2;

    element_init_same_as(id, test.get_Zn());
    element_init_same_as(msg, test.get_GT());
    element_init_same_as(res, test.get_GT());
    element_init_same_as(s1, test.get_Zn());
    element_init_same_as(s2, test.get_Zn());

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
    abe.KGen(skid, st, mpk, msk, id, attr_list);
    test.end("KeyGen");
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

    test.start("KUpt");
    abe.KUpt(kut, mpk, st, rl, T);
    test.end("KUpt");
    printf("size of kut.ku_theta: %ld\n", kut.get_ku_theta().size());

    test.start("DKGen");
    abe.DKGen(dkidt, mpk, skid, kut);
    test.end("DKGen");

    element_random(msg);
    Logger::PrintPbc("msg", msg);
    element_random(s1);
    element_random(s2);

    test.start("Encrypt");
    abe.Enc(ciphertext, mpk, msg, POLICY, T, s1, s2);
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

    test.start("Decrypt");
    abe.Dec(res, mpk, ciphertext, dkidt);
    test.end("Decrypt");

    Logger::PrintPbc("msg", msg);
    Logger::PrintPbc("res", res);

    if(element_cmp(msg, res) == 0){
        printf("Decrypt successfully.\n");
        test_result = 0;
    }else{
        printf("Decrypt failed.\n");
    } 

    test.start("Rev");
    time_t target_time = TimeUtils::TimeCast(2025, 12, 31, 0, 0, 0);
    abe.Rev(rl, id, target_time);
    test.end("Rev");
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

