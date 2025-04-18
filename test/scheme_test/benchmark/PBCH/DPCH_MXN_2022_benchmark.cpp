#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class DPCH_MXN_2022_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
            if(visiable)printf("%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {256, 512, 1024};
    int lamudas[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            for (int lamuda : lamudas) {
                test_params.push_back({curve, authNum, lamuda});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    DPCH_MXN_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(DPCH_MXN_2022_Test, Test){    
    DPCH_MXN_2022 ch(GetParam().curve);

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    Element_t_matrix *MSP = ch.ComputeMSP(POLICY);

    // attributes
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

    DPCH_MXN_2022_pp ppDPCH;
    DPCH_MXN_2022_pk pkDPCH;
    DPCH_MXN_2022_sk skDPCH;
    DPCH_MXN_2022_skGid skGid[repeat];
    DPCH_MXN_2022_sigmaGid sigmaGid;
    DPCH_MXN_2022_h h[repeat];
    DPCH_MXN_2022_r r[repeat], r_p[repeat];
    std::string m = "message";
    std::string m_p = "message_p";

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(ppDPCH, pkDPCH, skDPCH, GetParam().lamuda);
    this->end("SetUp");

    this->start("ModSetUp");
    for (int i = 0; i < repeat; i++) ch.ModSetUp(skGid[i], sigmaGid, skDPCH, GID);
    this->end("ModSetUp");
    
    {
        DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
        DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;
        this->start("AuthSetUp");
        for (int i = 0; i < repeat; i++) ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[0]);
        this->end("AuthSetUp");
        delete pkTheta;
        delete skTheta;
    }

    for(int j = 0; j < GetParam().authNum; j++) {
        DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
        DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;

        if(j < SIZE_OF_ATTRIBUTES) ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[j]);
        else ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, "ATTRIBUTE@AUTHORITY_" + std::to_string(j));

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);
    }
    // average("AuthSetUp", GetParam().authNum);
    
    {
        DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;
        this->start("ModKeyGen");
        for (int i = 0; i < repeat; i++) ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[0], ATTRIBUTES[0]);
        this->end("ModKeyGen");
        delete skGidA;
    }
    
    for(int j = 0; j < GetParam().authNum; j++) {
        DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;

        if (j < SIZE_OF_ATTRIBUTES) ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[j], ATTRIBUTES[j]);
        else ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[j], "ATTRIBUTE@AUTHORITY_" + std::to_string(j));
        
        skGidAs.push_back(skGidA);
    }
    // average("ModKeyGen", GetParam().authNum);

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m, ppDPCH, pkDPCH, pkThetas, MSP, POLICY);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(pkDPCH, m, h[i], r[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    // choose partial secret keys
    std::vector<DPCH_MXN_2022_skGidA *> _skGidAs;
    for (int i = 0; i < repeat; i++){
        _skGidAs.push_back(skGidAs[0]);
        _skGidAs.push_back(skGidAs[1]);
        _skGidAs.push_back(skGidAs[2]);
    }

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p, h[i], r[i], m, pkDPCH, skGid[i], _skGidAs, ppDPCH, pkThetas, MSP, POLICY);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(pkDPCH, m_p, h[i], r_p[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
    // std::vector<std::string> list = {"SetUp", "ModSetUp", "Hash", "Check", "Adapt", "Verify"};
    average();

    // free
    delete MSP;
    for(int j=0; j<pkThetas.size(); j++) delete pkThetas[j];
    for(int j=0; j<skThetas.size(); j++) delete skThetas[j];
    for(int j=0; j<skGidAs.size(); j++) delete skGidAs[j];
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}