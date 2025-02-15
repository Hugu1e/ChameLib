#include "ABE/MA_ABE.h"
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    MA_ABE abe(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;   // Policy的属性个数（不去重）

    // attribute
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<MA_ABE_pkTheta *> pkThetas;
    std::vector<MA_ABE_skTheta *> skThetas;
    std::vector<MA_ABE_skgidA *> skgidAs;

    MA_ABE_gpk gpk;

    element_t msg,res;
    element_t rt;
    MA_ABE_ciphertext c;

    element_init_same_as(msg, test.get_GT());
    element_init_same_as(res, test.get_GT());
    element_init_same_as(rt, test.get_Zn());

    test.start("GlobalSetup");
    abe.GlobalSetup(gpk);
    test.end("GlobalSetup");
    // Logger::PrintPbc("gpk.g", gpk.getElement("g"));

    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        MA_ABE_pkTheta *pkTheta = new MA_ABE_pkTheta();
        MA_ABE_skTheta *skTheta = new MA_ABE_skTheta();

        test.start("AuthSetup");
        abe.AuthSetup(*pkTheta, *skTheta, gpk, ATTRIBUTES[_]);
        test.end("AuthSetup");

        // Logger::PrintPbc("pkTheta.pkTheta_1", pkTheta->getElement("pkTheta_1"));
        // Logger::PrintPbc("pkTheta.pkTheta_2", pkTheta->getElement("pkTheta_2"));
        // Logger::PrintPbc("skTheta.aTheta", skTheta->getElement("aTheta"));
        // Logger::PrintPbc("skTheta.yTheta", skTheta->getElement("yTheta"));

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);
    }

    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        MA_ABE_skgidA *skgidA = new MA_ABE_skgidA();

        test.start("KeyGen");
        abe.KeyGen(*skgidA, gpk, *skThetas[_], GID, ATTRIBUTES[_]);
        test.end("KeyGen");

        // Logger::PrintPbc("skgidA.skgidA_0", skgidA->getElement("skgidA_0"));
        // Logger::PrintPbc("skgidA.skgidA_1", skgidA->getElement("skgidA_1"));

        skgidAs.push_back(skgidA);
    }

    element_random(msg);
    Logger::PrintPbc("msg", msg);
    element_random(rt);
    Logger::PrintPbc("rt", rt);

    test.start("Encrypt");
    abe.Encrypt(c, msg, rt, gpk, pkThetas, POLICY);
    test.end("Encrypt");
    // Logger::PrintPbc("c0", c.getC0()->getElement("c0"));
    // for(int i = 0;i < SIZE_OF_POLICY;i++){
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_1", c.getCi(i)->getElement("ci_1"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_2", c.getCi(i)->getElement("ci_2"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_3", c.getCi(i)->getElement("ci_3"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_4", c.getCi(i)->getElement("ci_4"));
    // }
    
    std::vector<MA_ABE_skgidA *> _skgidAs;  // partial secret keys
    for(int i = 0;i < skgidAs.size();i++) {
        _skgidAs.push_back(skgidAs[i]);
    }

    test.start("Decrypt");
    abe.Decrypt(res, _skgidAs, c);
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

