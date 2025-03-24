#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int lamuda;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "lamuda=" << params.lamuda;
}

const TestParams test_values[] = {
    {128},
    {256},
    {512},
    {1024},
    {2048}
};

class CH_ET_BC_CDK_2017_Test :public BaseTest<TestParams>{
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
    CH_ET_BC_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_ET_BC_CDK_2017_Test, Test){
    CH_ET_BC_CDK_2017 ch;

    CH_ET_BC_CDK_2017_pp pp[repeat];
    CH_ET_BC_CDK_2017_pk pk[repeat];
    CH_ET_BC_CDK_2017_sk sk[repeat];
    CH_ET_BC_CDK_2017_etd etd[repeat];
    CH_ET_BC_CDK_2017_h h[repeat];
    CH_ET_BC_CDK_2017_r r[repeat], r_p[repeat];

    std::string m[repeat], m_p[repeat];

    for (int i = 0; i < repeat; i++) {
        m[i] = "123456";
        m_p[i] = "789101";
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pp[i], GetParam().lamuda);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(pk[i], sk[i], pp[i]);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], etd[i], pp[i], pk[i], m[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], r[i], pk[i], m[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], sk[i], etd[i], pk[i], h[i], r[i], m[i], m_p[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], pk[i], m_p[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();
}

int main(int argc, char **argv) 
{
    if (argc > 1) {
        repeat = std::atoi(argv[1]);
        if (repeat <= 0) {
            std::cerr << "Invalid value for repeat. It must be a positive integer." << std::endl;
            return 1;
        }
    }
    
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

