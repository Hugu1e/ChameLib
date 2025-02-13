#include <scheme/PBCH/DPCH_MXN_2022.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    DPCH_MXN_2022 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;
    // attribute
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);
    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<DPCH_MXN_2022_pkTheta *> pkThetas;
    std::vector<DPCH_MXN_2022_skTheta *> skThetas;
    std::vector<DPCH_MXN_2022_skGidA *> skGidAs;
    int k = 1024;
    DPCH_MXN_2022_pp ppDPCH;
    DPCH_MXN_2022_pk pkDPCH;
    DPCH_MXN_2022_sk skDPCH;
    DPCH_MXN_2022_skGid skGid;
    DPCH_MXN_2022_sigmaGid sigmaGid;
    DPCH_MXN_2022_h h;
    DPCH_MXN_2022_r r,r_p;
    std::string m = "message";
    std::string m_p = "message_p";

    printf("k = %d\n", k);
    test.start("SetUp");
    ch.SetUp(ppDPCH, pkDPCH, skDPCH, h, r, r_p, sigmaGid, skGid, k);
    test.end("SetUp");
    ppDPCH.get_gpk_MA_ABE().print();
    pkDPCH.get_pk_CH().print();
    skDPCH.get_sk_CH().print();
    pkDPCH.get_pk_BLS().print();
    skDPCH.get_sk_BLS().print();


    test.start("ModSetUp");
    ch.ModSetUp(skGid, sigmaGid, skDPCH, GID);
    test.end("ModSetUp");
    skGid.get_sk_CH().print();
    sigmaGid.get_signature().print();


    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
        DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;

        test.start("AuthSetUp");
        ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[_]);
        test.end("AuthSetUp");
        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);

        pkTheta->get_pk().print();
        skTheta->get_sk().print();
    }

    for(int _ = 0;_ < SIZE_OF_ATTRIBUTES;_++) {
        DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;

        test.start("ModKeyGen");
        ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[_], ATTRIBUTES[_]);
        test.end("ModKeyGen");

        skGidAs.push_back(skGidA);

        printf("skGidA->gid: %s\n", skGidA->get_sk().get_gid().c_str());
        printf("skGidA->A: %s\n", skGidA->get_sk().get_A().c_str());
        skGidA->get_sk().print();
    skGidA->get_sk().print();
    }

    test.start("Hash");
    ch.Hash(h, r, m, ppDPCH, pkDPCH, pkThetas, POLICY);
    test.end("Hash");
    // h0, h1, r0, r1
    h.get_h().print();
    r.get_r().print();


    test.start("Check");
    bool check_result = ch.Check(pkDPCH, m, h, r);
    test.end("Check");
    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }


    // skGidAs.pop_back();
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pkDPCH, skGid, skGidAs, ppDPCH, pkThetas, POLICY);
    test.end("Adapt");
    // r0, r1
    r_p.get_r().print();



    test.start("Verify");
    bool verify_result = ch.Verify(pkDPCH, m_p, h, r_p);
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