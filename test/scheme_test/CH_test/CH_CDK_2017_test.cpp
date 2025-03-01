#include "scheme/CH/CH_CDK_2017.h"
#include "CommonTest.h"

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {256},
    {512},
    {1024},
    {2048}
};

class CH_CDK_2017_Test : public BaseTest<TestParams>{
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
	CH_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_CDK_2017_Test, Test){
    CH_CDK_2017 ch;

    CH_CDK_2017_pk pk;
    CH_CDK_2017_sk sk;
    CH_CDK_2017_h h;
    CH_CDK_2017_r r, r_p;
    mpz_t m, tag, m_p, tag_p;
    mpz_inits(m, tag, m_p, tag_p, NULL);

    this->start("SetUp");
    ch.SetUp(pk, sk, h, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().lamuda);
    this->end("KeyGen");

    RandomGenerator::RandomN(m, pk.get_rsa_pk()[AE_RSA::n]);
    RandomGenerator::RandomN(tag, pk.get_rsa_pk()[AE_RSA::n]);
    if(visiable){
        Logger::PrintGmp("m", m);
        Logger::PrintGmp("tag", tag);
    }
    this->start("Hash");
    ch.Hash(h, r, m, tag, pk);
    this->end("Hash");
    if(visiable){
        h.print();
        r.print();
    }

    this->start("Check");
    bool check_result = ch.Check(h, r, m, tag, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    RandomGenerator::RandomN(m_p, pk.get_rsa_pk()[AE_RSA::n]);
    if(visiable){
        Logger::PrintGmp("m_p", m_p);
    }
    this->start("Adapt");
    ch.Adapt(r_p, tag_p, h, r, m_p, sk, pk);
    this->end("Adapt");
    if(visiable){
        r_p.print();
        Logger::PrintGmp("tag_p", tag_p);
    }

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, tag_p, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

