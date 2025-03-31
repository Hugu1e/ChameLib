#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve];
}

class CH_KEF_MH_SDH_DL_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s\n", testName.c_str(), curveName.c_str());
            if(visiable)printf("%s %s\n", testName.c_str(), curveName.c_str());
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_SDH_DL_AM_2004_Test,
	testing::ValuesIn(test_values)
);

int op_cnt[][diff_max_len] = {
    {
        1, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 1, 
        0, 0, 0, 2, 
        2, 0, 0, 0, 
        4, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        2, 0, 0, 0, 
        2, 0, 0, 0, 
        2
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        1, 0, 0, 1, 
        1, 0, 0, 0, 
        0
    }, //4, adapt
};

TEST_P(CH_KEF_MH_SDH_DL_AM_2004_Test, Test){
    CH_KEF_MH_SDH_DL_AM_2004 ch(GetParam().curve);

    CH_KEF_MH_SDH_DL_AM_2004_pp pp[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_sk sk[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_pk pk[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_h h[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_r r[repeat], r_p[repeat];

    element_s *m[repeat], *m_p[repeat], *label[repeat];

    for (int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
        label[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for(int i=0; i<repeat; i++) ch.SetUp(pp[i]);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], pp[i]);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], label[i], pk[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], m[i], label[i], pk[i], pp[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], r[i], m[i], label[i], sk[i], pp[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], m_p[i], label[i], pk[i], pp[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();

    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[0], "SetUp"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[1], "KeyGen"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[2], "Hash"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Check"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[4], "Adapt"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Verify"));
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






