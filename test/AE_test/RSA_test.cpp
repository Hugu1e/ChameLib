#include "AE/RSA.h"
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
    RSA_pk pk;
    RSA_sk sk;

    mpz_t m,c,m2;
    mpz_inits(m,c,m2,NULL);

    this->start("SetUp");
    rsa.SetUp(pk, sk);
    this->end("SetUp");

    this->start("KeyGen");
    rsa.KeyGen(pk, sk, GetParam().k);
    this->end("KeyGen");
    if(visiable){
        pk.print();
        sk.print();
    }

    mpz_set_ui(m, 123456);
    this->start("Encrypt");
    rsa.Encrypt(c, m, pk);
    this->end("Encrypt");
    if(visiable)gmp_printf("Ciphertext: %Zd\n", c);

    this->start("Decrypt");
    rsa.Decrypt(m2, c, sk, pk);
    this->end("Decrypt");
    if(visiable)gmp_printf("Decrypted Plaintext: %Zd\n", m);

    bool result = mpz_cmp(m, m2) == 0;
    mpz_clears(m,c,m2,NULL);
    ASSERT_TRUE(result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}