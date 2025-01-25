#include <ABE/ABET.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    ABET abe(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    std::vector<std::string> attr_list = {"ONE","TWO","THREE","FOUR"};
    const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

    const int K = 10;
    const int I = 5;  // modifier
    const int J = 5;  // owner
    ABET_ID id;
    ABET_mpk mpk;
    ABET_msk msk;
    ABET_sks sks;

    element_t r,R, res_r, res_R;
    element_t s1,s2;
    ABET_ciphertext ciphertext;

    for(int i = 1;i<=K;i++){
        element_t tmp_Zn;
        element_init_same_as(tmp_Zn, test.get_Zn());
        element_random(tmp_Zn);

        id.insertElement("I"+std::to_string(i), "Zn", tmp_Zn);
        element_clear(tmp_Zn);
    }
    id.printElement();

    element_init_same_as(r, test.get_Zn());
    element_init_same_as(R, test.get_Zn());
    element_init_same_as(res_r, test.get_Zn());
    element_init_same_as(res_R, test.get_Zn());

    element_init_same_as(s1, test.get_Zn());
    element_init_same_as(s2, test.get_Zn());

    test.start("Setup");
    abe.Setup(&msk, &mpk, K);
    test.end("Setup");
    // msk.printElement();
    // mpk.printElement();

    test.start("KeyGen");
    abe.KeyGen(&sks, &msk, &mpk, &attr_list, &id, I);
    test.end("KeyGen");
    // Logger::PrintPbc("sk0.sk0_1", sks.get_sk0()->getElement("sk0_1"));
    // Logger::PrintPbc("sk0.sk0_2", sks.get_sk0()->getElement("sk0_2"));
    // Logger::PrintPbc("sk0.sk0_3", sks.get_sk0()->getElement("sk0_3"));
    // Logger::PrintPbc("sk0.sk0_4", sks.get_sk0()->getElement("sk0_4"));
    // Logger::PrintPbc("sk0.sk0_5", sks.get_sk0()->getElement("sk0_5"));
    // Logger::PrintPbc("sk0.sk0_6", sks.get_sk0()->getElement("sk0_6"));
    // for(int i = 0;i < SIZE_OF_ATTR;i++){
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_1", sks.get_sk_y(i)->getElement("sk_1"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_2", sks.get_sk_y(i)->getElement("sk_2"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_3", sks.get_sk_y(i)->getElement("sk_3"));
    // }
    // Logger::PrintPbc("sk_prime.sk_1", sks.get_sk_prime()->getElement("sk_1"));
    // Logger::PrintPbc("sk_prime.sk_2", sks.get_sk_prime()->getElement("sk_2"));
    // Logger::PrintPbc("sk_prime.sk_3", sks.get_sk_prime()->getElement("sk_3"));
    // Logger::PrintPbc("sk1", sks.get_sk1()->getElement("sk1"));
    // for(int i = 1;i <= I-1;i++){
    //     Logger::PrintPbc("sk2[" + std::to_string(i) + "]", sks.get_sk2()->getElement("sk2_"+std::to_string(i)));
    // }

    element_random(r);
    element_random(R);
    Logger::PrintPbc("R", R);
    Logger::PrintPbc("r", r);

    element_random(s1);
    element_random(s2);

    test.start("Encrypt");
    abe.Encrypt(&ciphertext, &mpk, &msk, r, R, POLICY, &id, J, s1, s2);
    test.end("Encrypt");
    // Logger::PrintPbc("ct0.ct_1", ciphertext.get_ct0()->getElement("ct0_1"));
    // Logger::PrintPbc("ct0.ct_2", ciphertext.get_ct0()->getElement("ct0_2"));
    // Logger::PrintPbc("ct0.ct_3", ciphertext.get_ct0()->getElement("ct0_3"));
    // Logger::PrintPbc("ct0.ct_4", ciphertext.get_ct0()->getElement("ct0_4"));
    // for(int i = 0;i < SIZE_OF_POLICY;i++){
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.get_ct_y(i)->getElement("ct_1"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.get_ct_y(i)->getElement("ct_2"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.get_ct_y(i)->getElement("ct_3"));
    // }
    // Logger::PrintPbc("ct_", ciphertext.get_ct_()->getElement("ct_"));
    // Logger::PrintPbc("ct_prime", ciphertext.get_ct_prime()->getElement("ct_prime"));
    // Logger::PrintPbc("ct1", ciphertext.get_ct1()->getElement("ct1"));
    // Logger::PrintPbc("ct2", ciphertext.get_ct2()->getElement("ct2"));
    // Logger::PrintPbc("ct3", ciphertext.get_ct3()->getElement("ct3"));

    test.start("Decrypt");
    abe.Decrypt(res_R, res_r, &mpk, &ciphertext, &sks);
    test.end("Decrypt");
    Logger::PrintPbc("res_R", res_R);
    Logger::PrintPbc("res_r", res_r);

    if(element_cmp(R,res_R) == 0  && element_cmp(r,res_r) == 0){
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

