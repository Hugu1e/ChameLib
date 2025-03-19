#include "scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k;
    int tau;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "k=" << params.k << " tau=" << params.tau;
}

const TestParams test_values[] = {
    {1024, 512}
};

class CH_KEF_MH_RSA_F_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            int k = GetParam().k;
            int tau = GetParam().tau;
            fprintf(out, "%s k %d tau %d\n", testName.c_str(), k, tau);
            if(visiable)printf("%s k %d tau %d\n", testName.c_str(), k, tau);
        }
};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSA_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSA_F_AM_2004_Test, Test){
    CH_KEF_MH_RSA_F_AM_2004 ch;

    CH_KEF_MH_RSA_F_AM_2004_pk pk[repeat];
    CH_KEF_MH_RSA_F_AM_2004_sk sk[repeat];
    mpz_t h[repeat], B[repeat], m[repeat], m_p[repeat], label[repeat], r[repeat], r_p[repeat];
    for (int i = 0; i < repeat; i++) mpz_inits(h[i], B[i], r[i], m[i], label[i], r_p[i], m_p[i], NULL);

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp();
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], GetParam().k, GetParam().tau);
    this->end("KeyGen");

    for (int i = 0; i < repeat; i++) {
        mpz_set_ui(m[i], 42525346346746 + i);
        mpz_set_ui(label[i], 424253532414 + i);
    }

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], B[i], pk[i], sk[i], m[i], label[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(pk[i], m[i], label[i], h[i], r[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    for (int i = 0; i < repeat; i++) mpz_set_ui(m_p[i], 96725346346246 + i);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], pk[i], m[i], m_p[i], label[i], h[i], B[i], r[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(pk[i], m_p[i], label[i], h[i], r_p[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

