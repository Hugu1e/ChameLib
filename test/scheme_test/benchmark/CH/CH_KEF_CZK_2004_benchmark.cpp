#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve] << " group=" << params.group;
}

class CH_KEF_CZK_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            std::string groupName = Curve::group_names[GetParam().group];
            fprintf(out, "%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
            if(visiable)printf("%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
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

    int groups[] = {Group::G1, Group::G2, Group::GT};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int group : groups) {
            test_params.push_back({curve, group});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_CZK_2004_Test,
	testing::ValuesIn(test_values)
);

int op_cnt_G1[][diff_max_len] = {
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
        0, 0, 0, 0, 
        2, 0, 0, 0, 
        3, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        2, 0, 0, 0, 
        1, 0, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        4, 0, 0, 1, 
        2, 0, 0, 0, 
        0
    }, //4, adapt
};


int op_cnt_G2[][diff_max_len] = {
    {
        0, 1, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 1, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 2, 0, 0, 
        0, 3, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 2, 0, 0, 
        0, 1, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 4, 0, 1, 
        0, 2, 0, 0, 
        0
    }, //4, adapt
};

int op_cnt_GT[][diff_max_len] = {
    {
        0, 0, 1, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 1, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 2, 0, 
        0, 0, 3, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 2, 0, 
        0, 0, 1, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 4, 1, 
        0, 0, 2, 0, 
        0
    }, //4, adapt
};

TEST_P(CH_KEF_CZK_2004_Test, Test){
    CH_KEF_CZK_2004 ch(GetParam().curve, GetParam().group);
    CH_KEF_CZK_2004_pp pp[repeat];
    CH_KEF_CZK_2004_pk pk[repeat];
    CH_KEF_CZK_2004_sk sk[repeat];
    CH_KEF_CZK_2004_r r[repeat], r_p[repeat];
    element_s *I[repeat], *h[repeat], *m[repeat], *m_p[repeat];

    for(int i = 0; i < repeat; i++) {
        I[i] = ch.GetG1Element();
        h[i] = ch.GetG1Element();
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for(int i = 0; i < repeat; i++) ch.SetUp(pp[i]);
    this->end("SetUp");

    this->start("KeyGen");
    for(int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], pp[i]);
    this->end("KeyGen");

    this->start("Hash");
    for(int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], I[i], pk[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for(int i = 0; i < repeat; i++) check_result[i] = ch.Check(I[i], m[i], r[i], h[i], pp[i]);
    this->end("Check");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for(int i = 0; i < repeat; i++) ch.Adapt(r_p[i], sk[i], h[i], m[i], r[i], m_p[i], I[i], pp[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for(int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(I[i], m_p[i], r_p[i], h[i], pp[i]);
    this->end("Verify");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();

    if(GetParam().group == Group::G1){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[3], "Verify"));
    } else if(GetParam().group == Group::G2){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G2[3], "Verify"));
    } else if(GetParam().group == Group::GT){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[3], "Verify"));
    }

    // free
    for(int i = 0; i < repeat; i++) {
        element_clear(I[i]);
        element_clear(h[i]);
        element_clear(m[i]);
        element_clear(m_p[i]);
        delete I[i];
        delete h[i];
        delete m[i];
        delete m_p[i];
    }
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}




