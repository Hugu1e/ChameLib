#include "scheme/CH/CH_KEF_MH_RSANN_F_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k;
};

const TestParams test_values[] = {
    {512}
};

class CH_KEF_MH_RSANN_F_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            int k = GetParam().k;
            fprintf(out, "%s k %d\n", testName.c_str(), k);
            if(visiable)printf("%s k %d\n", testName.c_str(), k);
        }
};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSANN_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSANN_F_AM_2004_Test, Test){
    CH_KEF_MH_RSANN_F_AM_2004 ch;

    CH_KEF_MH_RSANN_F_AM_2004_pk pk;
    CH_KEF_MH_RSANN_F_AM_2004_sk sk;
    CH_KEF_MH_RSANN_F_AM_2004_h h;
    CH_KEF_MH_RSANN_F_AM_2004_r r, r_p;
    
    mpz_t m,m_p;
    mpz_t label;
    
    mpz_inits(m, m_p, label, NULL);
    RandomGenerator::RandomInLength(m, 64);
    RandomGenerator::RandomInLength(m_p, 64);
    RandomGenerator::RandomInLength(label, 64);
    
    if(visiable){
        Logger::PrintGmp("m", m);
        Logger::PrintGmp("m_p", m_p);
        Logger::PrintGmp("label", label);
    }

    this->start("SetUp");
    ch.SetUp(pk, sk, h, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().k);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, m, label, pk);
    this->end("Hash");
    if(visiable){
        h.print();
        r.print();
    }    

    this->start("Check");
    bool check_result = ch.Check(h, m, r, label, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    mpz_set_ui(m_p, 96725346346246);
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, label, sk, pk);
    this->end("Adapt");
    if(visiable)r_p.print();

    this->start("Verify");
    bool verify_result = ch.Verify(h, m_p, r_p, label, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


