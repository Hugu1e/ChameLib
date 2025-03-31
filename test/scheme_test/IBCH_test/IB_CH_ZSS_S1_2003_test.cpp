#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve] << " swap=" << params.swap;
}

class IB_CH_ZSS_S1_2003_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            if(visiable)printf("%s %s swap:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::D_159, Curve::D_201, Curve::D_224, Curve::D_105171_196_185, Curve::D_277699_175_167, Curve::D_278027_190_181,
        Curve::E,
        Curve::F, Curve::SM9,
        Curve::G_149
    };

    bool swaps[] = {false, true};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	IBCH_Test,
    IB_CH_ZSS_S1_2003_Test,
	testing::ValuesIn(test_values)
);

int op_cnt[][diff_max_len] = {
    {
        0, 1, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0,  
        0, 1, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 0, 
        1, 0, 0, 0, 
        0, 0, 0, 0,  
        1, 0, 0, 0, 
        0
    }, //1, Extract
    
    {
        1, 0, 0, 0, 
        1, 0, 0, 1, 
        0, 0, 1, 0,  
        1, 0, 0, 0, 
        2
    }, //2, hash

    {
        0, 0, 0, 0, 
        1, 0, 0, 1, 
        0, 0, 1, 0,  
        1, 0, 0, 0, 
        2
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        0, 0, 0, 0,  
        1, 0, 0, 0, 
        0
    }, //4, adapt
};

int op_cnt_swap[][diff_max_len] = {
    {
        1, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0,  
        1, 0, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 0, 0,  
        0, 1, 0, 0, 
        0
    }, //1, Extract
    
    {
        0, 1, 0, 0, 
        0, 1, 0, 1, 
        0, 0, 1, 0,  
        0, 1, 0, 0, 
        2
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 1, 0, 1, 
        0, 0, 1, 0,  
        0, 1, 0, 0, 
        2
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        0, 0, 0, 0,  
        0, 1, 0, 0, 
        0
    }, //4, adapt
};

TEST_P(IB_CH_ZSS_S1_2003_Test, Test){
    IB_CH_ZSS_S1_2003 ch(GetParam().curve, GetParam().swap);

    IB_CH_ZSS_S1_2003_pp pp[repeat];
    IB_CH_ZSS_S1_2003_msk msk[repeat];
    IB_CH_ZSS_S1_2003_pk pk[repeat];
    IB_CH_ZSS_S1_2003_sk sk[repeat];
    IB_CH_ZSS_S1_2003_h h[repeat];
    IB_CH_ZSS_S1_2003_r r[repeat], r_p[repeat];

    element_s *m[repeat], *m_p[repeat], *ID[repeat];

    for (int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
        ID[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pp[i], msk[i]);
    this->end("SetUp");

    this->start("Extract");
    for (int i = 0; i < repeat; i++) ch.Extract(pk[i], sk[i], msk[i], ID[i]);
    this->end("Extract");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], ID[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], m[i], r[i], ID[i], pp[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], m[i], r[i], ID[i], sk[i], pp[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], m_p[i], r_p[i], ID[i], pp[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();

    if(!GetParam().swap){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[1], "Extract"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Verify"));
    }else{
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[1], "Extract"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_swap[3], "Verify"));
    }
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



