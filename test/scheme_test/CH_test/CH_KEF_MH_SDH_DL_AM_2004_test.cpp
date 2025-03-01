#include "scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

class CH_KEF_MH_SDH_DL_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s\n", testName.c_str(), curveName.c_str());
            if(visiable)printf("%s %s\n", testName.c_str(), curveName.c_str());
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_SDH_DL_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_SDH_DL_AM_2004_Test, Test){
    CH_KEF_MH_SDH_DL_AM_2004 ch(GetParam().curve);
        
    CH_KEF_MH_SDH_DL_AM_2004_pp pp;
    CH_KEF_MH_SDH_DL_AM_2004_sk sk;
    CH_KEF_MH_SDH_DL_AM_2004_pk pk;
    CH_KEF_MH_SDH_DL_AM_2004_h h;
    CH_KEF_MH_SDH_DL_AM_2004_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *label = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    if(visiable){
        Logger::PrintPbc("label", label);
        Logger::PrintPbc("m", m);
    }
    this->start("Hash");
    ch.Hash(h, r, m, label, pk, pp);
    this->end("Hash");
    if(visiable){
        Logger::PrintPbc("Hash value", h[0]);
        Logger::PrintPbc("r", r[0]);
    }

    this->start("Check");
    bool check_result = ch.Check(h, r, m, label, pk, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, label, sk, pp);
    this->end("Adapt");
    if(visiable){
        Logger::PrintPbc("m_p", m_p);
        Logger::PrintPbc("r_p", r_p[0]);
    }

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, label, pk, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






