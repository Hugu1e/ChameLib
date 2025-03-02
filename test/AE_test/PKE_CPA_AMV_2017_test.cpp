#include "AE/PKE_CPA_AMV_2017.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class PKE_CPA_AMV_2017_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            visiable = true;

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
	AE_Test,
    PKE_CPA_AMV_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PKE_CPA_AMV_2017_Test, Test){
    PKE_CPA_AMV_2017 pke(GetParam().curve, GetParam().group);

    PKE_CPA_AMV_2017_pk pk;
    PKE_CPA_AMV_2017_sk sk;
    PKE_CPA_AMV_2017_c c;

    element_s *m = pke.GetZrElement();
    element_s *res = pke.GetZrElement();

    this->start("Setup");
    pke.Setup(pk, sk, c);
    this->end("Setup");

    this->start("KeyGen");
    pke.KeyGen(pk, sk);
    this->end("KeyGen");

    if(visiable)Logger::PrintPbc("m", m);
    this->start("Encrypt");
    pke.Encrypt(c, m, pk);
    this->end("Encrypt");

    this->start("Decrypt");
    pke.Decrypt(res, c, sk);
    this->end("Decrypt");
    if(visiable)Logger::PrintPbc("res", res);

    bool verify_result = element_cmp(m, res) == 0;
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}