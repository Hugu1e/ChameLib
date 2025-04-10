#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
    int k;
};

class RPCH_XNM_2021_Test : public BaseTest<TestParams>{
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

    int leadNodeSizes[] = {1024, 2048, 4096};

    int ks[] = {256, 512, 1024};

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
    RPCH_XNM_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RPCH_XNM_2021_Test, Test){
    RPCH_XNM_2021 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE", "TWO", "THREE"};
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // Compute MSP
    Element_t_matrix* MSP = ch.ComputeMSP(POLICY);

    element_s *id_2[repeat];
    for (int i = 0; i < repeat; i++) {
        id_2[i] = ch.GetZrElement();
    }

    // T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // Present time
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);

    RPCH_XNM_2021_pp ppRPCH[repeat];
    RPCH_XNM_2021_sk skRPCH[repeat];
    RPCH_XNM_2021_pk pkRPCH[repeat];

    RPCH_XNM_2021_skid skidRPCH_2[repeat];

    RPCH_XNM_2021_dkidt dkidtRPCH_2[repeat];

    RPCH_XNM_2022_kut kut[repeat];
    RPCH_XNM_2021_h h[repeat];
    RPCH_XNM_2021_r r[repeat], r_p[repeat];

    RPCH_XNM_2021_RevokedPresonList rl[repeat];
    RPCH_XNM_2022_Binary_tree st[repeat];

    std::string m[repeat], m_p[repeat];
    for (int i = 0; i < repeat; i++) {
        m[i] = "message to hash" + std::to_string(i);
        m_p[i] = "message to adapt" + std::to_string(i);
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(ppRPCH[i], skRPCH[i], pkRPCH[i], rl[i], st[i], GetParam().k, GetParam().leafNodeSize);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(skidRPCH_2[i], pkRPCH[i], skRPCH[i], st[i], attr_list, id_2[i], re_time_2);
    this->end("KeyGen");

    this->start("Rev");
    for (int i = 0; i < repeat; i++) ch.Rev(rl[i], id_2[i], re_time_2);
    this->end("Rev");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], pkRPCH[i], MSP, T_present, ppRPCH[i]);
    this->end("Hash");

    bool check[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check[i] = ch.Check(h[i], r[i], m[i], pkRPCH[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check[i]);

    this->start("KUpt");
    for (int i = 0; i < repeat; i++) ch.KUpt(kut[i], pkRPCH[i], st[i], rl[i], T_present);
    this->end("KUpt");

    this->start("DKGen");
    for (int i = 0; i < repeat; i++) ch.DKGen(dkidtRPCH_2[i], pkRPCH[i], skidRPCH_2[i], kut[i]);
    this->end("DKGen");

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], r[i], m[i], pkRPCH[i], dkidtRPCH_2[i], MSP);
    this->end("Adapt");

    bool verify[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify[i] = ch.Verify(h[i], r_p[i], m_p[i], pkRPCH[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify[i]);

    average();

    // free
    delete MSP;
    for(int i = 0; i < repeat; i++) {
        element_clear(id_2[i]);
        delete id_2[i];
    }
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);
    
    ::testing::GTEST_FLAG(catch_exceptions) = false;
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}