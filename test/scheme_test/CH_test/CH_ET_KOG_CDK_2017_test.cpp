#include "scheme/CH/CH_ET_KOG_CDK_2017.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class CH_ET_KOG_CDK_2017_Test : public BaseTest<TestParams>{
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
    CH_ET_KOG_CDK_2017_Test,
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
        0, 0, 0, 2, 
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        2, 0, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 4, 
        0, 0, 0, 3, 
        2, 0, 0, 2, 
        7, 0, 0, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        4, 0, 0, 0, 
        7, 0, 0, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 1, 
        0, 0, 0, 4, 
        4, 0, 0, 6, 
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
        0, 0, 0, 2, 
        0, 0, 0, 1, 
        0, 0, 0, 1, 
        0, 0, 2, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 4, 
        0, 0, 0, 3, 
        0, 0, 2, 2, 
        0, 0, 7, 0, 
        0
    }, //2, hash

    {
        0, 0, 0, 0, 
        0, 0, 0, 3, 
        0, 0, 4, 0, 
        0, 0, 7, 0, 
        0
    }, //3, check

    {
        0, 0, 0, 1, 
        0, 0, 0, 4, 
        0, 0, 4, 6, 
        0, 0, 11, 0, 
        0
    }, //4, adapt
};
TEST_P(CH_ET_KOG_CDK_2017_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        CH_ET_KOG_CDK_2017 ch(GetParam().curve, GetParam().group);

        CH_ET_KOG_CDK_2017_pp pp;
        CH_ET_KOG_CDK_2017_sk sk;
        CH_ET_KOG_CDK_2017_pk pk;
        CH_ET_KOG_CDK_2017_h h;
        CH_ET_KOG_CDK_2017_r r, r_p;
        CH_ET_KOG_CDK_2017_etd etd;

        element_s* m = ch.GetZrElement();
        element_s* m_p = ch.GetZrElement();

        this->start("SetUp");
        ch.SetUp(pp);
        this->end("SetUp");

        this->start("KeyGen");
        ch.KeyGen(sk, pk, pp, 1024);
        this->end("KeyGen");

        this->start("Hash");
        ch.Hash(h, r, etd, m, pk, pp);
        this->end("Hash");
        if(visiable){
            h.get_hash().print();
            r.get_ch_r().print();
            r.get_enc_c().print();
        }

        this->start("Check");
        bool check = ch.Check(h, m, r, pk, pp);
        this->end("Check");
        ASSERT_TRUE(check);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, m, r, pk, pp, sk, etd);
        this->end("Adapt");
        if(visiable){
            r_p.get_ch_r().print();
            r_p.get_enc_c().print();
        }

        this->start("Verify");
        bool verify = ch.Verify(h, m_p, r_p, pk, pp);
        this->end("Verify");
        ASSERT_TRUE(verify);
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



