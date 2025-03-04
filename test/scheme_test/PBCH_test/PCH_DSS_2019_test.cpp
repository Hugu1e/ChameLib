#include "scheme/PBCH/PCH_DSS_2019.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCH_DSS_2019_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().k);
            if(visiable)printf("%s %s swap: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().k);
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::D_159, Curve::D_201, Curve::D_224, Curve::D_105171_196_185, Curve::D_277699_175_167, Curve::D_278027_190_181,
        Curve::E,
        Curve::F, Curve::SM9,
        Curve::G_149
    };

    bool swaps[] = {false, true};

    int ks[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int k : ks) {
                test_params.push_back({curve, swap, k});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    PCH_DSS_2019_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCH_DSS_2019_Test, Test){
    PCH_DSS_2019 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    PCH_DSS_2019_sk skPCH;
    PCH_DSS_2019_pk pkPCH;
    PCH_DSS_2019_sks sksPCH;

    mpz_t m,m_p;
    PCH_DSS_2019_h h;
    PCH_DSS_2019_r r,r_p;

    mpz_inits(m,m_p,NULL);

    RandomGenerator::RandomInLength(m, 128);
    RandomGenerator::RandomInLength(m_p, 128);
    if(visiable){
        Logger::PrintGmp("m", m);
        Logger::PrintGmp("m_p", m_p);
    }

    this->start("SetUp");
    ch.SetUp(pkPCH, skPCH, h, r, r_p, GetParam().k);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(skPCH, pkPCH, attr_list, sksPCH);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(pkPCH, m, POLICY, h, r);
    this->end("Hash");
    if(visiable){
        // h1 h2 N2
        h.getH().print();
    }

    this->start("Check");
    bool check_result = ch.Check(pkPCH, m, h, r);
    this->end("Check");
    ASSERT_TRUE(check_result);
    
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pkPCH, sksPCH, POLICY);
    this->end("Adapt");
    if(visiable){
        r_p.print();
    }

    this->start("Verify");
    bool verify_result = ch.Verify(pkPCH, m_p, h, r_p);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
    ::testing::GTEST_FLAG(catch_exceptions) = false;
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}