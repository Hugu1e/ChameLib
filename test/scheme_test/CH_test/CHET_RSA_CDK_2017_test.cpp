#include "scheme/CH/CHET_RSA_CDK_2017.h"
#include "CommonTest.h"

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {128},
    {256},
    {512},
};

class CHET_RSA_CDK_2017_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            int lamuda = GetParam().lamuda;
            fprintf(out, "%s lamuda %d\n", testName.c_str(), lamuda);
            if(visiable)printf("%s lamuda %d\n", testName.c_str(), lamuda);
        }
};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CHET_RSA_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CHET_RSA_CDK_2017_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        CHET_RSA_CDK_2017 ch;
        CHET_RSA_CDK_2017_pk pk;
        CHET_RSA_CDK_2017_sk sk;
        CHET_RSA_CDK_2017_h h;
        CHET_RSA_CDK_2017_etd etd;

        mpz_t r, r_p;
        mpz_t m, m_p;
        mpz_inits(r, r_p, m, m_p, NULL);

        this->start("SetUp");
        ch.SetUp(pk, sk, h, etd);
        this->end("SetUp");

        this->start("KeyGen");
        ch.KeyGen(pk, sk, GetParam().lamuda);
        this->end("KeyGen");
        if(visiable){
            pk.print();
            sk.print();
        }

        RandomGenerator::RandomInLength(m, 100);
        this->start("Hash");
        ch.Hash(h, r, etd, m, pk);
        this->end("Hash");
        if(visiable){
            Logger::PrintGmp("m", m);
            h.print();
            Logger::PrintGmp("r", r);
            etd.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, m, pk);
        this->end("Check");
        ASSERT_TRUE(check_result);

        RandomGenerator::RandomInLength(m_p, 100);
        this->start("Adapt");
        ch.Adapt(r_p, m_p, m, r, h, sk, etd, pk);
        this->end("Adapt");
        if(visiable){
            Logger::PrintGmp("m_p", m_p);
            Logger::PrintGmp("r_p", r_p);
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, r_p, m_p, pk);
        this->end("Verify");
        ASSERT_TRUE(verify_result);
    }
    average();
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

