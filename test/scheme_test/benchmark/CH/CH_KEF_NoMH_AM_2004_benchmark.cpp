#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int k;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "k=" << params.k;
}

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
    CH_KEF_NoMH_AM_2004 ch;

    CH_KEF_NoMH_AM_2004_pk pk[repeat];
    CH_KEF_NoMH_AM_2004_sk sk[repeat];
    CH_KEF_NoMH_AM_2004_h h[repeat];
    CH_KEF_NoMH_AM_2004_r r[repeat], r_p[repeat];
    mpz_t m[repeat], m_p[repeat];

    for (int i = 0; i < repeat; i++) mpz_inits(m[i], m_p[i], NULL);
    for (int i = 0; i < repeat; i++) RandomGenerator::RandomInLength(m[i], 1024);
    for (int i = 0; i < repeat; i++) RandomGenerator::RandomInLength(m_p[i], 1024);

    this->start("Setup");
    for (int i = 0; i < repeat; i++) ch.Setup();
    this->end("Setup");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], GetParam().k);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], pk[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], m[i], pk[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], r[i], m[i], pk[i], sk[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], m_p[i], pk[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();

    // free
    for (int i = 0; i < repeat; i++) mpz_clears(m[i], m_p[i], NULL);
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);
    
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


