#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int k;
};

class RSA_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            fprintf(out, "%s k: %d \n", testName.c_str(), GetParam().k);
            if(visiable)printf("%s k: %d \n", testName.c_str(), GetParam().k);
        }
};

std::vector<TestParams> generateTestParams() {
    int ks[] = {
        256, 512, 1024, 2048
    };

    std::vector<TestParams> test_params;

    for (int k : ks) {
        test_params.push_back({k});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	AE_Test,
    RSA_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RSA_Test, Test){
    AE_RSA rsa;
    RSA_pk pk[repeat];
    RSA_sk sk[repeat];

    mpz_t m[repeat], c[repeat], res[repeat];
    for(int i = 0; i < repeat; i++) mpz_inits(m[i], c[i], res[i], NULL);


    this->start("KeyGen");
    for(int i = 0; i < repeat; i++) rsa.KeyGen(pk[i], sk[i], GetParam().k);
    this->end("KeyGen");

    for(int i = 0; i < repeat; i++) RandomGenerator::RandomInLength(m[i], 128);

    this->start("Encrypt");
    for(int i = 0; i < repeat; i++) rsa.Encrypt(c[i], m[i], pk[i]);
    this->end("Encrypt");

    this->start("Decrypt");
    for(int i = 0; i < repeat; i++) rsa.Decrypt(res[i], c[i], sk[i], pk[i]);
    this->end("Decrypt");

    for(int i = 0; i < repeat; i++) ASSERT_TRUE(mpz_cmp(m[i], res[i]) == 0);

    for(int i = 0; i < repeat; i++) mpz_clears(m[i], c[i], res[i], NULL);
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}