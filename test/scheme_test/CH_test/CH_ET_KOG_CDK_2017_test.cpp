#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve] << " group=" << Curve::group_names[params.group];
}

class CH_ET_KOG_CDK_2017_Test : public BaseTest<TestParams>{
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
    CH_ET_KOG_CDK_2017_Test,
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
        0, 0, 0, 2, 
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        2, 0, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 4, 
        0, 0, 0, 4, 
        2, 0, 0, 2, 
        7, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 4, 
        4, 0, 0, 0, 
        7, 0, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 1, 
        0, 0, 0, 7, 
        4, 0, 0, 6, 
        11, 0, 0, 0, 
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
        0, 0, 0, 2, 
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        0, 2, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 4, 
        0, 0, 0, 4, 
        0, 2, 0, 2, 
        0, 7, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 4, 
        0, 4, 0, 0, 
        0, 7, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 1, 
        0, 0, 0, 7, 
        0, 4, 0, 6, 
        0, 11, 0, 0, 
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
        0, 0, 0, 2, 
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        0, 0, 2, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 4, 
        0, 0, 0, 4, 
        0, 0, 2, 2, 
        0, 0, 7, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 4, 
        0, 0, 4, 0, 
        0, 0, 7, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 1, 
        0, 0, 0, 7, 
        0, 0, 4, 6, 
        0, 0, 11, 0, 
        0
    }, //4, adapt
};

TEST_P(CH_ET_KOG_CDK_2017_Test, Test){
    CH_ET_KOG_CDK_2017 ch(GetParam().curve, GetParam().group);

    CH_ET_KOG_CDK_2017_pp pp[repeat];
    CH_ET_KOG_CDK_2017_sk sk[repeat];
    CH_ET_KOG_CDK_2017_pk pk[repeat];
    CH_ET_KOG_CDK_2017_h h[repeat];
    CH_ET_KOG_CDK_2017_r r[repeat], r_p[repeat];
    CH_ET_KOG_CDK_2017_etd etd[repeat];

    element_s* m[repeat];
    element_s* m_p[repeat];
    for (int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pp[i]);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(sk[i], pk[i], pp[i], 1024);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], etd[i], m[i], pk[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], m[i], r[i], pk[i], pp[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], m[i], r[i], pk[i], pp[i], sk[i], etd[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], m_p[i], r_p[i], pk[i], pp[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
    average();
    if(GetParam().group == Group::G1){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1[3], "Verify"));
    }else if(GetParam().group == Group::G2){
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
}

int main(int argc, char **argv) 
{
    if (argc > 1) {
        repeat = std::atoi(argv[1]);
        if (repeat <= 0) {
            std::cerr << "Invalid value for repeat. It must be a positive integer." << std::endl;
            return 1;
        }
    }

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



