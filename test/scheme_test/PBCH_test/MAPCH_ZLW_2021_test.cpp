#include "scheme/PBCH/MAPCH_ZLW_2021.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class MAPCH_ZLW_2021_Test : public BaseTest<TestParams>{
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
    int authNums[] = {16, 32, 64};
    // int lamudas[] = {256, 512, 1024};
    int lamudas[] = {32, 64, 128};

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
    MAPCH_ZLW_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MAPCH_ZLW_2021_Test, Test){
    MAPCH_ZLW_2021 ch(GetParam().curve);

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

    this->start("SetUp");
    
    this->start("GlobalSetup");
    ch.GlobalSetup(pkCH, skCH, gpkABE, pp, h, r, r_p, GetParam().lamuda);
    this->end("GlobalSetup");
    
    this->start("AuthSetUp");
    for(int i=0; i<GetParam().authNum; i++){
        MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
        MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();

        if (i < SIZE_OF_ATTRIBUTES) {
            ch.AuthSetUp(*mhk, *mtk, As[i], pkCH, skCH, gpkABE, pp);
        }else{
            ch.AuthSetUp(*mhk, *mtk, "ATTRIBUTE@AUTHORITY_" + std::to_string(i), pkCH, skCH, gpkABE, pp);
        }
        
        mhks.push_back(mhk);
        mtks.push_back(mtk);
    }
    this->end("AuthSetUp");
    this->end("SetUp");

    this->start("KeyGen");
    for(int i=0; i<GetParam().authNum; i++){
        MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();
        if(i < SIZE_OF_ATTRIBUTES){
            ch.KeyGen(*mski, *mtks[i], *mhks[i], As[i], GID);
        }else{
            ch.KeyGen(*mski, *mtks[i], *mhks[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i), GID);
        }
        mskis.push_back(mski);
    }
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, m, pp, mhks, POLICY);
    this->end("Hash");
    if(visiable){
        h.get_h().print();
        r.get_r().print();
    }

    this->start("Check");
    bool check_result = ch.Check(h, r, m, mhks);
    this->end("Check");
    ASSERT_TRUE(check_result);

    // mskis.pop_back();
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, mhks, mskis);
    this->end("Adapt");
    if(visiable){
        r_p.get_r().print();
    }

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, mhks);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}