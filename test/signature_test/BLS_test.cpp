#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int swap;
};

class BLS_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            if(visiable)printf("%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	Signature_Test,
    BLS_Test,
	testing::ValuesIn(test_values)
);

TEST_P(BLS_Test, Test){
    BLS bls(GetParam().curve, GetParam().swap);
    BLS_pp pp;
    BLS_pk pk;
    BLS_sk sk;
    BLS_signature signature;
    
    this->start("Setup");
    bls.Setup(pp);
    this->end("Setup");

    this->start("KeyGen");
    bls.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    std::string message = "Hello, BLS!";
    this->start("Sign");
    bls.Sign(signature, sk, message);
    this->end("Sign");

    this->start("Verify");
    bool verify_result = bls.Verify(pp, pk, message, signature);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}