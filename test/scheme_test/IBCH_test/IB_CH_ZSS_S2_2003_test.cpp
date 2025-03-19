#include "scheme/IBCH/IB_CH_ZSS_S2_2003.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve];
}

class IB_CH_ZSS_S2_2003_Test : public BaseTest<TestParams>{
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
	IBCH_Test,
    IB_CH_ZSS_S2_2003_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_ZSS_S2_2003_Test, Test){
    IB_CH_ZSS_S2_2003 ch(GetParam().curve);

    IB_CH_ZSS_S2_2003_pp pp[repeat];
    IB_CH_ZSS_S2_2003_msk msk[repeat];
    IB_CH_ZSS_S2_2003_sk sk[repeat];
    IB_CH_ZSS_S2_2003_h h[repeat];
    IB_CH_ZSS_S2_2003_r r[repeat], r_p[repeat];

    element_s *m[repeat], *m_p[repeat], *ID[repeat];

    for (int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
        ID[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pp[i], msk[i]);
    this->end("SetUp");

    this->start("Extract");
    for (int i = 0; i < repeat; i++) ch.Extract(sk[i], msk[i], ID[i], pp[i]);
    this->end("Extract");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], ID[i], pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], m[i], r[i], ID[i], pp[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], m[i], r[i], ID[i], sk[i], pp[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], m_p[i], r_p[i], ID[i], pp[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();
}


int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}