#include "scheme/IBCH/IB_CH_MD_LSX_2022.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
};

class IB_CH_MD_LSX_2022_Test : public BaseTest<TestParams>{
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
    IB_CH_MD_LSX_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_MD_LSX_2022_Test, Test){
    IB_CH_MD_LSX_2022 ch(GetParam().curve);

    IB_CH_MD_LSX_2022_pp pp;
    IB_CH_MD_LSX_2022_msk msk;
    IB_CH_MD_LSX_2022_td td;
    IB_CH_MD_LSX_2022_h h;
    IB_CH_MD_LSX_2022_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *ID = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pp, msk, td, h, r, r_p);
    this->end("SetUp");
    if(visiable){
        pp.print();
        msk.print();
    }

    this->start("KeyGen");
    ch.KeyGen(td, ID, msk, pp);
    this->end("KeyGen");
    if(visiable){
        Logger::PrintPbc("ID", ID);
        td.print();
    }
    
    
    this->start("Hash");
    ch.Hash(h, r, ID, m, pp);
    this->end("Hash");
    if(visiable){
        Logger::PrintPbc("m", m);
        h.print();
        r.print();
    }

    this->start("Check");
    bool check_result = ch.Check(h, r, ID, m, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("Adapt");
    ch.Adapt(r_p, h, m, r, m_p, td);
    this->end("Adapt");
    if(visiable){    
        Logger::PrintPbc("m_p", m_p);
        r_p.print();
    }

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, ID, m_p, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






