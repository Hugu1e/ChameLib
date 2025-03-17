#include "scheme/CH/CH_KEF_DLP_LLA_2012.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class CH_KEF_DLP_LLA_2012_Test : public BaseTest<TestParams>{
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

    // int groups[] = {Group::G1, Group::G2, Group::GT};
    int groups[] = {Group::GT};

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
    CH_KEF_DLP_LLA_2012_Test,
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
        1, 0, 0, 3, 
        0, 0, 0, 1, 
        1, 0, 0, 0, 
        6, 0, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        2, 0, 0, 0, 
        3, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 1, 
        2, 0, 0, 0, 
        3, 0, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        3, 0, 0, 3, 
        5, 0, 0, 0, 
        0
    }, //4, Uforge
    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        0, 0, 0, 0, 
        0
    }, //5, Iforge
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
        0, 0, 1, 3, 
        0, 0, 0, 1, 
        0, 0, 1, 0, 
        0, 0, 6, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        0, 0, 2, 0, 
        0, 0, 3, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 1, 
        0, 0, 2, 0, 
        0, 0, 3, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        0, 0, 3, 3, 
        0, 0, 5, 0, 
        0
    }, //4, Uforge
    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        0, 0, 0, 0, 
        0
    }, //5, Iforge
};

TEST_P(CH_KEF_DLP_LLA_2012_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        CH_KEF_DLP_LLA_2012 ch(GetParam().curve, GetParam().group);

        CH_KEF_DLP_LLA_2012_pp pp;
        CH_KEF_DLP_LLA_2012_sk sk;
        CH_KEF_DLP_LLA_2012_pk pk;
        CH_KEF_DLP_LLA_2012_label label;
            
        element_s *m = ch.GetZrElement();
        element_s *m_p = ch.GetZrElement();
        element_s *m_pp = ch.GetZrElement();
        element_s *r = ch.GetZrElement();
        element_s *r_p = ch.GetZrElement();
        element_s *r_pp = ch.GetZrElement();
        element_s *h = ch.GetG1Element();

        this->start("SetUp");
        ch.SetUp(pp);
        this->end("SetUp");
        if(visiable){
            pp.print();
        }

        this->start("KeyGen");
        ch.KeyGen(sk, pk, label, pp);
        this->end("KeyGen");
        if(visiable){
            sk.print();
            pk.print();
        }

        this->start("Hash");
        ch.Hash(h, r, pk, m, label, pp);
        this->end("Hash");
        if(visiable){
            Logger::PrintPbc("m", m);
            Logger::PrintPbc("h", h);
            Logger::PrintPbc("r", r);
        }

        this->start("Check");
        bool check_result = ch.Check(m, r, pk, label, h, pp);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("UForge");
        ch.UForge(r_p, sk, pk, label, h, m, m_p, r, pp);
        this->end("UForge");
        if(visiable){
            Logger::PrintPbc("m_p", m_p);
            Logger::PrintPbc("r_p", r_p);
        }

        this->start("Verify");
        bool verify_result_1 = ch.Verify(m_p, r_p, pk, label, h, pp);
        this->end("Verify");
        ASSERT_TRUE(verify_result_1);

        this->start("IForge");
        ch.IForge(r_pp, label, m, m_p, r, r_p, m_pp);
        this->end("IForge");
        if(visiable){
            Logger::PrintPbc("m_pp", m_pp);
            Logger::PrintPbc("r_pp", r_pp);
        }

        this->start("Verify");
        bool verify_result_2 = ch.Verify(m_pp, r_pp, pk, label, h, pp);
        this->end("Verify");
        ASSERT_TRUE(verify_result_2);
    }
    average();
    if(GetParam().group == Group::G1 || GetParam().group == Group::G2){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[4], "UForge"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_G1G2[5], "IForge"));
    }else if(GetParam().group == Group::GT){
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[0], "SetUp"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[1], "KeyGen"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[2], "Hash"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[3], "Check"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[4], "UForge"));
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GT[5], "IForge"));
    }
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






