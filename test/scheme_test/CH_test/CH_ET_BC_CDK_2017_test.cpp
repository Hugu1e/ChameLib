#include "scheme/CH/CH_ET_BC_CDK_2017.h"
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
    CH_ET_BC_CDK_2017_pp pp;
    CH_ET_BC_CDK_2017_pk pk;
    CH_ET_BC_CDK_2017_sk sk;
    CH_ET_BC_CDK_2017_etd etd;
    CH_ET_BC_CDK_2017_h h;
    CH_ET_BC_CDK_2017_r r,r_p;

    std::string m = "123456";
    std::string m_p = "789101";

    this->start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p, etd, GetParam().lamuda);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, etd, pp, pk, m);
    this->end("Hash");
    if(visiable)h.print();

    this->start("Check");
    bool check_result = ch.Check(h, r, pk, m);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("Adapt");
    ch.Adapt(r_p, sk, etd, pk, h, r, m, m_p);
    this->end("Adapt");
    if(visiable)r.print();
    
    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, pk, m_p);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

