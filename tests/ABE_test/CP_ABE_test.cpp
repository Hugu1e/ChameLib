#include <ABE/CP_ABE.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S, S是Policy所有属性的子集
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

    CP_ABE abe(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());
    CP_ABE_mpk mpk;
    CP_ABE_msk msk;
    CP_ABE_sks sks;
    CP_ABE_ciphertext ciphertext;

    element_t msg,res;
    

    element_init_same_as(msg, test.get_GT());
    element_init_same_as(res, test.get_GT());
    
    test.start("Setup");
    abe.Setup(msk, mpk);
    test.end("Setup");
    // msk.printElement();
    // mpk.printElement();

    test.start("KeyGen");
    abe.KeyGen(sks, msk, mpk, attr_list);
    test.end("KeyGen");
    // Logger::PrintPbc("sk0.sk_1", sks.get_sk0()->getElement("sk_1"));
    // Logger::PrintPbc("sk0.sk_2", sks.get_sk0()->getElement("sk_2"));
    // Logger::PrintPbc("sk0.sk_3", sks.get_sk0()->getElement("sk_3"));
    // for(int i = 0;i < SIZE_OF_ATTR;i++){
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_1", sks.get_sk_y(i)->getElement("sk_1"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_2", sks.get_sk_y(i)->getElement("sk_2"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_3", sks.get_sk_y(i)->getElement("sk_3"));
    // }
    // Logger::PrintPbc("sk_prime.sk_1", sks.get_sk_prime()->getElement("sk_1"));
    // Logger::PrintPbc("sk_prime.sk_2", sks.get_sk_prime()->getElement("sk_2"));
    // Logger::PrintPbc("sk_prime.sk_3", sks.get_sk_prime()->getElement("sk_3"));

    element_random(msg);
    Logger::PrintPbcWithSize("msg", msg);
    test.start("Encrypt");
    abe.Encrypt(ciphertext, mpk, msg, POLICY);
    test.end("Encrypt");
    Logger::PrintPbc("ct0.ct_1", ciphertext.get_ct0()[CP_ABE::ct_1]);
    Logger::PrintPbc("ct0.ct_2", ciphertext.get_ct0()[CP_ABE::ct_2]);
    Logger::PrintPbc("ct0.ct_3", ciphertext.get_ct0()[CP_ABE::ct_3]);
    for(int i = 0;i < SIZE_OF_POLICY;i++){
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.get_ct_y(i)[CP_ABE::ct_1]);
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.get_ct_y(i)[CP_ABE::ct_2]);
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.get_ct_y(i)[CP_ABE::ct_3]);
    }
    Logger::PrintPbc("ct_prime", ciphertext.get_ct_prime()[0]);

    test.start("Decrypt");
    abe.Decrypt(res, ciphertext, mpk, sks);
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

