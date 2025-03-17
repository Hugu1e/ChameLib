#include "scheme/CH/CR_CH_DSS_2020.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class CR_CH_DSS_2020_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            std::string groupName;
            if (GetParam().group == Group::G1) {
                groupName = "G1";
            } else if (GetParam().group == Group::G2) {
                groupName = "G2";
            } else if (GetParam().group == Group::GT) {
                groupName = "GT";
            } else {
                groupName = "UNKNOWN";
            }
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
    CR_CH_DSS_2020_Test,
	testing::ValuesIn(test_values)
);

int op_cnt_G1G2[][diff_max_len] = {
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
        0, 0, 0, 4, 
        0, 0, 0, 1, 
        2, 0, 0, 1, 
        6, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 1, 
        4, 0, 0, 0, 
        6, 0, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 3, 
        0, 0, 0, 2, 
        7, 0, 0, 1, 
        11, 0, 0, 0, 
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
        0, 0, 0, 4, 
        0, 0, 0, 1, 
        0, 0, 2, 1, 
        0, 0, 6, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 1, 
        0, 0, 4, 0, 
        0, 0, 6, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 3, 
        0, 0, 0, 2, 
        0, 0, 7, 1, 
        0, 0, 11, 0, 
        0
    }, //4, adapt
};
TEST_P(CR_CH_DSS_2020_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        CR_CH_DSS_2020 ch(GetParam().curve, GetParam().group);

        CR_CH_DSS_2020_pp pp;
        CR_CH_DSS_2020_pk pk;
        CR_CH_DSS_2020_sk sk;
        CR_CH_DSS_2020_h h;
        CR_CH_DSS_2020_r r,r_p;

        element_s *m = ch.GetG1Element();
        element_s *m_p = ch.GetG1Element();

        this->start("SetUp");
        ch.SetUp(pp);
        this->end("SetUp");
        if(visiable){
            pp.print();
        }

        this->start("KeyGen");
        ch.KeyGen(pk, sk, pp);
        this->end("KeyGen");
        if(visiable){
            pk.print();
            sk.print();
        }

        this->start("Hash");
        ch.Hash(h, r, m, pk, pp);
        this->end("Hash");
        if(visiable){
            Logger::PrintPbc("m", m);
            h.print();
            r.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, m, pk, pp);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, h, r, m, m_p, sk, pk, pp);
        this->end("Adapt");
        if(visiable){
            Logger::PrintPbc("m_p", m_p);
            r_p.print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, r_p, m_p, pk, pp);
        this->end("Verify");
        ASSERT_TRUE(verify_result);
    }
    average();

    if(GetParam().group == Group::G1 || GetParam().group == Group::G2){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[4], "Adapt"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[3], "Verify"));
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
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






