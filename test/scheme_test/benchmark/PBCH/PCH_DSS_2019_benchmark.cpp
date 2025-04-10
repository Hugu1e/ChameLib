#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCH_DSS_2019_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().k);
            if(visiable)printf("%s %s swap: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().k);
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

    int ks[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int k : ks) {
                test_params.push_back({curve, swap, k});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    PCH_DSS_2019_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCH_DSS_2019_Test, Test){
    PCH_DSS_2019 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    Element_t_matrix* MSP = ch.ComputeMSP(POLICY);

    PCH_DSS_2019_pp ppPCH[repeat];
    PCH_DSS_2019_sk skPCH[repeat];
    PCH_DSS_2019_pk pkPCH[repeat];
    PCH_DSS_2019_sks sksPCH[repeat];

    PCH_DSS_2019_h h[repeat];
    PCH_DSS_2019_r r[repeat], r_p[repeat];

    std::string m = "message to hash";
    std::string m_p = "message to adapt";

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(ppPCH[i], pkPCH[i], skPCH[i], GetParam().k);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(sksPCH[i], skPCH[i], pkPCH[i], attr_list);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m, MSP, POLICY, pkPCH[i], ppPCH[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], m, pkPCH[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);
    
    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p, h[i], r[i], m, sksPCH[i], pkPCH[i], MSP, POLICY);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], m_p, pkPCH[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
    average();

    // free
    delete MSP;
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);
    
    ::testing::GTEST_FLAG(catch_exceptions) = false;
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}