#include "scheme/CH/CH_KEF_NoMH_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k;
};

const TestParams test_values[] = {
    {512}
};

class CH_KEF_NoMH_AM_2004_Test : public BaseTest<TestParams>{
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
    CH_KEF_NoMH_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_NoMH_AM_2004_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        CH_KEF_NoMH_AM_2004 ch = CH_KEF_NoMH_AM_2004();

        CH_KEF_NoMH_AM_2004_pk pk;
        CH_KEF_NoMH_AM_2004_sk sk;
        CH_KEF_NoMH_AM_2004_h h;
        CH_KEF_NoMH_AM_2004_r r, r_p;

        mpz_t m,m_p;
        mpz_inits(m, m_p, NULL);
        RandomGenerator::RandomInLength(m, 1024);
        RandomGenerator::RandomInLength(m_p, 1024);

        this->start("Setup");
        ch.Setup();
        this->end("Setup");

        this->start("KeyGen");
        ch.KeyGen(pk, sk, GetParam().k);
        this->end("KeyGen");
        if(visiable){
            pk.print();
            sk.print();
        }

        this->start("Hash");
        ch.Hash(h, r, m, pk);
        this->end("Hash");
        if(visiable){
            Logger::PrintGmp("m", m);
            h.print();
            r.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, m, pk);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, r, m, pk, sk);
        this->end("Adapt");
        if(visiable){
            Logger::PrintGmp("m_p", m_p);
            r_p.print();
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


