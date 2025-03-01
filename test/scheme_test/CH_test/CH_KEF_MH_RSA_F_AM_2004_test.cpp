#include "scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k;
    int tau;
};

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

    CH_KEF_MH_RSA_F_AM_2004_pk pk;
    CH_KEF_MH_RSA_F_AM_2004_sk sk;
    mpz_t h;
    mpz_t B;
    mpz_t m,m_p;
    mpz_t label;
    mpz_t r, r_p;
    mpz_inits(h, B, r, m, label, r_p, m_p, NULL);

    this->start("SetUp");
    ch.SetUp(pk, sk);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().k, GetParam().tau);
    this->end("KeyGen");

    mpz_set_ui(m, 42525346346746);
    mpz_set_ui(label, 424253532414);
    this->start("Hash");
    ch.Hash(h, r, B, pk, sk, m, label);
    this->end("Hash");
    if(visiable){
        Logger::PrintGmp("h", h);
        Logger::PrintGmp("r", r);
        Logger::PrintGmp("B", B);
    }

    
    this->start("Check");
    bool check_result = ch.Check(pk, m, label, h, r);
    this->end("Check");
    ASSERT_TRUE(check_result);

    mpz_set_ui(m_p, 96725346346246);
    this->start("Adapt");
    ch.Adapt(r_p, pk, m, m_p, label, h, B, r);
    this->end("Adapt");
    if(visiable)Logger::PrintGmp("r_p", r_p);

    this->start("Verify");
    bool verify_result = ch.Verify(pk, m_p, label, h, r_p);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

