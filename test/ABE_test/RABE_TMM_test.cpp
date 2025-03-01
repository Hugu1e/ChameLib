#include "ABE/RABE_TMM.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
};

class RABE_TMM_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
            if(visiable)printf("%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                test_params.push_back({curve, swap, leafNodeSize});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	ABE_Test,
    RABE_TMM_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RABE_TMM_Test, Test){
    RABE_TMM abe(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    element_s *id1 = abe.GetZrElement();
    element_s *id2 = abe.GetZrElement();
    element_s *id3 = abe.GetZrElement();

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);

    RABE_TMM_mpk mpk;
    RABE_TMM_msk msk;
    RABE_TMM_skid skid1, skid2, skid3;
    RABE_TMM_kut kut;
    RABE_TMM_dkidt dkidt1, dkidt2, dkidt3;
    RABE_TMM_ciphertext ciphertext;

    std::vector<RABE_TMM_revokedPreson> rl;
    Binary_tree_RABE st;

    element_s *msg = abe.GetZrElement();
    element_s *res = abe.GetZrElement();
    element_s *s1 = abe.GetZrElement();
    element_s *s2 = abe.GetZrElement();

    this->start("Setup");
    abe.Setup(mpk, msk, rl, st, GetParam().leafNodeSize);
    this->end("Setup");

    this->start("KeyGen");
    abe.KGen(skid1, st, mpk, msk, attr_list, id1, re_time_1);
    this->end("KeyGen");
    abe.KGen(skid2, st, mpk, msk, attr_list, id2, re_time_2);
    abe.KGen(skid3, st, mpk, msk, attr_list, id3, re_time_3);

    this->start("Rev");
    abe.Rev(rl, id1, re_time_1);
    this->end("Rev");
    abe.Rev(rl, id2, re_time_2);
    abe.Rev(rl, id3, re_time_3);

    if(visiable) Logger::PrintPbc("msg", msg);
    this->start("Encrypt");
    abe.Enc(ciphertext, mpk, msg, POLICY, T_present, s1, s2);
    this->end("Encrypt");

    this->start("KUpt");
    abe.KUpt(kut, mpk, st, rl, T_present);
    this->end("KUpt");
    if(visiable) printf("size of kut.ku_theta: %ld\n", kut.get_ku_theta().size());

    this->start("DKGen");
    try{
        abe.DKGen(dkidt1, mpk, skid1, kut);
    }catch(const std::runtime_error& e){
        if(visiable) printf("%s\n", e.what());
    }
    this->end("DKGen");

    this->start("DKGen");
    try{
        abe.DKGen(dkidt3, mpk, skid3, kut);
    }catch(const std::runtime_error& e){
        if(visiable) printf("%s\n", e.what());
    }
    this->end("DKGen");

    this->start("DKGen");
    abe.DKGen(dkidt2, mpk, skid2, kut);
    this->end("DKGen");

    this->start("Decrypt");
    abe.Dec(res, mpk, ciphertext, dkidt2);
    this->end("Decrypt");

    if(visiable){
        Logger::PrintPbc("msg", msg);
        Logger::PrintPbc("res", res);
    }
    ASSERT_TRUE(element_cmp(msg, res) == 0);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}