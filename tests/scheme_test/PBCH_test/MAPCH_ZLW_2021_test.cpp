#include <scheme/PBCH/MAPCH_ZLW_2021.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    MAPCH_ZLW_2021 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());


    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    // attribute
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    std::vector<std::string> As = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = As.size();
    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";


    const int K = 256;
    MAPCH_ZLW_2021_pp pp;
    std::vector<MAPCH_ZLW_2021_mhk *> mhks;
    std::vector<MAPCH_ZLW_2021_mtk *> mtks;
    std::vector<MAPCH_ZLW_2021_mski *> mskis;
    MAPCH_ZLW_2021_h h,h_p;

    std::string m = "message";
    std::string m_p = "message_p";

    printf("k = %d\n", K);
        
    test.start("SetUp");
    ch.SetUp(&pp, &mhks, &mtks, K, &As);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(&mskis, &mtks, &mhks, &As, GID);
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(&h, &pp, &mhks, m, POLICY);
    test.end("Hash");
    h.getH()->printElement("h0");
    h.getH()->printElement("h1");
    h.getR()->printElement("r0");
    h.getR()->printElement("r1");

    test.start("Check");
    bool check_result = ch.Check(&mhks, m, &h);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    // mskis.pop_back();
    test.start("Adapt");
    ch.Adapt(&h_p, &mhks, &mskis, m, m_p, &h);
    test.end("Adapt");
    h_p.getH()->printElement("h0");
    h_p.getH()->printElement("h1");
    h_p.getR()->printElement("r0");
    h_p.getR()->printElement("r1");

    test.start("Verify");
    bool verify_result = ch.Verify(&mhks, m_p, &h_p);
    test.end("Verify");

    if(verify_result){
        printf("Verify successful!\n");
        test_result = 0;
    }else{
        printf("Verify failed.\n");
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