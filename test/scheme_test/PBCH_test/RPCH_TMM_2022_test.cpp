#include "scheme/PBCH/RPCH_TMM_2022.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
    int k;
};

class RPCH_TMM_2022_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d leafNodeSize: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize, GetParam().k);
            if(visiable)printf("%s %s swap: %d leafNodeSize: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize, GetParam().k);
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

    int leadNodeSizes[] = {8, 16, 32, 64};

    int ks[] = {128, 256, 512};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                for (int k : ks) {
                    test_params.push_back({curve, swap, leafNodeSize, k});
                }
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    RPCH_TMM_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RPCH_TMM_2022_Test, Test){
    RPCH_TMM_2022 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    element_s *id_1 = ch.GetZrElement();
    element_s *id_2 = ch.GetZrElement();
    element_s *id_3 = ch.GetZrElement();

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);
    
    RPCH_TMM_2022_sk skRPCH;
    RPCH_TMM_2022_pk pkRPCH;

    RPCH_TMM_2022_skid skidRPCH_1;
    RPCH_TMM_2022_skid skidRPCH_2;
    RPCH_TMM_2022_skid skidRPCH_3;

    RPCH_TMM_2022_dkidt dkidtRPCH_1;
    RPCH_TMM_2022_dkidt dkidtRPCH_2;
    RPCH_TMM_2022_dkidt dkidtRPCH_3;

    RPCH_TMM_2022_kut kut;

    RPCH_TMM_2022_h h,h_p;

    RPCH_TMM_2022_RevokedPresonList rl;
    RPCH_TMM_2022_Binary_tree st;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    
    this->start("SetUp");
    ch.SetUp(skRPCH, pkRPCH, rl, st, GetParam().k, GetParam().leafNodeSize);
    this->end("SetUp");
    
    this->start("KeyGen");
    ch.KeyGen(skidRPCH_1, pkRPCH, skRPCH, st, attr_list, id_1, re_time_1);
    this->end("KeyGen");
    ch.KeyGen(skidRPCH_2, pkRPCH, skRPCH, st, attr_list, id_2, re_time_2);
    ch.KeyGen(skidRPCH_3, pkRPCH, skRPCH, st, attr_list, id_3, re_time_3);

    this->start("Rev");
    ch.Rev(rl, id_1, re_time_1);
    this->end("Rev");
    ch.Rev(rl, id_2, re_time_2);
    ch.Rev(rl, id_3, re_time_3);

    this->start("Hash");
    ch.Hash(h, m, pkRPCH, POLICY, T_present);
    this->end("Hash");
        
    this->start("Check");
    bool check = ch.Check(pkRPCH, m, h);
    this->end("Check");
    ASSERT_TRUE(check);
    
    this->start("KUpt");
    ch.KUpt(kut, pkRPCH, st, rl, T_present);
    this->end("KUpt");
    
    try{
        ch.DKGen(dkidtRPCH_1, pkRPCH, skidRPCH_1, kut);
    }catch(const std::runtime_error& e){
        if(visiable) printf("%s\n", e.what());
    }
    try{
        ch.DKGen(dkidtRPCH_3, pkRPCH, skidRPCH_3, kut);
    }catch(const std::runtime_error& e){
        if(visiable) printf("%s\n", e.what());
    }
    this->start("DKGen");
    ch.DKGen(dkidtRPCH_2, pkRPCH, skidRPCH_2, kut);
    this->end("DKGen");

    this->start("Adapt");
    ch.Adapt(h_p, m_p, m, h, pkRPCH, dkidtRPCH_2, POLICY);
    this->end("Adapt");
    
    this->start("Verify");
    bool verify = ch.Verify(pkRPCH, m_p, h_p);
    this->end("Verify");
    ASSERT_TRUE(verify);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}