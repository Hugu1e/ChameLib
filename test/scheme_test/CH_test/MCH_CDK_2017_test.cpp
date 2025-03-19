#include "scheme/CH/MCH_CDK_2017.h"
#include "CommonTest.h"

struct TestParams{
	int lamuda;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "lamuda=" << params.lamuda;
}

const TestParams test_values[] = {
    {256},
    {512},
    {1024},
    {2048}
};

class MCH_CDK_2017_Test : public BaseTest<TestParams>{
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
    MCH_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MCH_CDK_2017_Test, Test){
    MCH_CDK_2017 ch;
    MCH_CDK_2017_pk pk[repeat];
    MCH_CDK_2017_sk sk[repeat];
    MCH_CDK_2017_h h[repeat];
    MCH_CDK_2017_r r[repeat], r_p[repeat];

    mpz_t m[repeat], m_p[repeat];
    for(int i = 0; i < repeat; i++){
        mpz_inits(m[i], m_p[i], NULL);
        RandomGenerator::RandomInLength(m[i], 100);
        RandomGenerator::RandomInLength(m_p[i], 100);
    }

    this->start("SetUp");
    ch.SetUp();
    this->end("SetUp");

    this->start("KeyGen");
    for(int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], GetParam().lamuda);
    this->end("KeyGen");

    this->start("Hash");
    for(int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], pk[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for(int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], m[i], pk[i]);
    this->end("Check");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);
    
    this->start("Adapt");
    for(int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], m[i], r[i], h[i], sk[i], pk[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for(int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], m_p[i], pk[i]);
    this->end("Verify");
    for(int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

