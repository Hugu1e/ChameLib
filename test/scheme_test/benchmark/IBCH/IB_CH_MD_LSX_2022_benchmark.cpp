#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve];
}

class IB_CH_MD_LSX_2022_Test : public BaseTest<TestParams>{
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
	IBCH_Test,
    IB_CH_MD_LSX_2022_Test,
	testing::ValuesIn(test_values)
);

int op_cnt[][diff_max_len] = {
    {
        3, 0, 0, 2, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        2
    }, //0, setup

    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 1, 
        1, 0, 0, 0, 
        0
    }, //1, keygen
    
    {
        1, 0, 0, 1, 
        0, 0, 0, 0, 
        1, 0, 2, 0, 
        1, 0, 2, 0, 
        1
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 2, 0, 
        1, 0, 2, 0, 
        1
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 0, 1, 
        1, 0, 0, 0, 
        0
    }, //4, adapt
};

TEST_P(IB_CH_MD_LSX_2022_Test, Test){
    IB_CH_MD_LSX_2022 ch(GetParam().curve);

    IB_CH_MD_LSX_2022_pp pp[repeat];
    IB_CH_MD_LSX_2022_msk msk[repeat];
    IB_CH_MD_LSX_2022_td td[repeat];
    IB_CH_MD_LSX_2022_h h[repeat];
    IB_CH_MD_LSX_2022_r r[repeat], r_p[repeat];

    element_s *m[repeat], *m_p[repeat], *ID[repeat];

    for(int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
        ID[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for(int i = 0; i < repeat; i++) ch.SetUp(pp[i], msk[i]);
    this->end("SetUp");

    this->start("KeyGen");
    for(int i = 0; i < repeat; i++) ch.KeyGen(td[i], ID[i], msk[i], pp[i]);
    this->end("KeyGen");

    this->start("Hash");
    for(int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], ID[i], m[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for(int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], ID[i], m[i], pp[i]);
    this->end("Check");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for(int i = 0; i < repeat; i++) ch.Adapt(r_p[i], h[i], m[i], r[i], m_p[i], td[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for(int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], ID[i], m_p[i], pp[i]);
    this->end("Verify");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
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






