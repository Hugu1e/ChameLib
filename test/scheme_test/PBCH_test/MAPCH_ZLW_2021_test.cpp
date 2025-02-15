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

    CH_ET_BC_CDK_2017_pk pkCH;
    CH_ET_BC_CDK_2017_sk skCH;
    MA_ABE_gpk gpkABE;

    std::vector<MAPCH_ZLW_2021_mhk *> mhks;
    std::vector<MAPCH_ZLW_2021_mtk *> mtks;
    std::vector<MAPCH_ZLW_2021_mski *> mskis;
    MAPCH_ZLW_2021_h h;
    MAPCH_ZLW_2021_r r,r_p;

    std::string m = "message";
    std::string m_p = "message_p";

    printf("k = %d\n", K);

    test.start("SetUp");
    
    test.start("GlobalSetup");
    ch.GlobalSetup(pkCH, skCH, gpkABE, pp, h, r, r_p, K);
    test.end("GlobalSetup");
    
    for(int i=0;i<SIZE_OF_ATTRIBUTES;i++){
        MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
        MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();
        test.start("AuthSetUp");
        ch.AuthSetUp(*mhk, *mtk, As[i], pkCH, skCH, gpkABE, pp);
        test.end("AuthSetUp");
        mhks.push_back(mhk);
        mtks.push_back(mtk);
    }
    test.end("SetUp");

    for(int i=0;i<SIZE_OF_ATTRIBUTES;i++){
        MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();
        test.start("KeyGen");
        ch.KeyGen(*mski, *mtks[i], *mhks[i], As[i], GID);
        test.end("KeyGen");
        mskis.push_back(mski);
    }
    

    test.start("Hash");
    ch.Hash(h, r, m, pp, mhks, POLICY);
    test.end("Hash");
    h.get_h().print();
    r.get_r().print();

    test.start("Check");
    bool check_result = ch.Check(h, r, m, mhks);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    // mskis.pop_back();
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, mhks, mskis);
    test.end("Adapt");
    r_p.get_r().print();

    test.start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, mhks);
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