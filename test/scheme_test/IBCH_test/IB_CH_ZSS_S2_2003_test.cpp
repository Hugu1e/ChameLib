#include "scheme/IBCH/IB_CH_ZSS_S2_2003.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

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
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        IB_CH_ZSS_S2_2003 ch(GetParam().curve);

        IB_CH_ZSS_S2_2003_pp pp;
        IB_CH_ZSS_S2_2003_msk msk;
        IB_CH_ZSS_S2_2003_sk sk;
        IB_CH_ZSS_S2_2003_h h;
        IB_CH_ZSS_S2_2003_r r,r_p;

        element_s *m = ch.GetZrElement();
        element_s *m_p = ch.GetZrElement();
        element_s *ID = ch.GetZrElement();

        this->start("SetUp");
        ch.SetUp(pp, msk);
        this->end("SetUp");
        if(visiable){
            pp.print();
            msk.print();
        }

        this->start("Extract");
        ch.Extract(sk, msk, ID, pp);
        this->end("Extract");
        if(visiable){
            Logger::PrintPbc("ID", ID);
            sk.print();
        }

        this->start("Hash");
        ch.Hash(h, r, m, ID, pp);
        this->end("Hash");
        if(visiable){
            Logger::PrintPbc("m", m);
            h.print();
            r.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, m, r, ID, pp);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, m, r, ID, sk, pp);
        this->end("Adapt");
        if(visiable){
            Logger::PrintPbc("m_p", m_p);
            r_p.print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, m_p, r_p, ID, pp);
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