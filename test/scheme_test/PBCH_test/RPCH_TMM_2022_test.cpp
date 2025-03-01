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
    
    const time_t T = TimeUtils::TimeCast(2024, 12, 21, 0, 0, 0);  // present time

    const time_t end_time_1 = TimeUtils::TimeCast(2025, 12, 31, 0, 0, 0);

    RPCH_TMM_2022_sk skRPCH;
    RPCH_TMM_2022_pk pkRPCH;
    RPCH_TMM_2022_skid skidRPCH;
    RPCH_TMM_2022_dkidt dkidtRPCH;
    RPCH_TMM_2022_kut kut;

    RPCH_TMM_2022_h h,h_p;

    RPCH_TMM_2022_RevokedPresonList rl;
    RPCH_TMM_2022_Binary_tree st;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *id = ch.GetZrElement();
    
    this->start("SetUp");
    ch.SetUp(skRPCH, pkRPCH, rl, st, GetParam().k, GetParam().leafNodeSize);
    this->end("SetUp");
    
    this->start("KeyGen");
    ch.KeyGen(skidRPCH, pkRPCH, skRPCH, st, attr_list, id, end_time_1);
    this->end("KeyGen");
    
    this->start("KUpt");
    ch.KUpt(kut, pkRPCH, st, rl, T);
    this->end("KUpt");
    
    this->start("DKGen");
    ch.DKGen(dkidtRPCH, pkRPCH, skidRPCH, kut);
    this->end("DKGen");
    
    this->start("Rev");
    ch.Rev(rl, id, T);
    this->end("Rev");

    this->start("Hash");
    ch.Hash(h, m, pkRPCH, POLICY, T);
    this->end("Hash");
    
    this->start("Check");
    bool check = ch.Check(pkRPCH, m, h);
    this->end("Check");
    ASSERT_TRUE(check);
        
    this->start("Adapt");
    ch.Adapt(h_p, m_p, m, h, pkRPCH, dkidtRPCH);
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