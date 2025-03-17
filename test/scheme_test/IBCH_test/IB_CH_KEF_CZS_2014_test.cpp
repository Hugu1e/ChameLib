#include "scheme/IBCH/IB_CH_KEF_CZS_2014.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
};

class IB_CH_KEF_CZS_2014_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            if(visiable)printf("%s %s swap:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

int op_cnt[][diff_max_len] = {
    {
        1, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //1, Extract
    
    {
        0, 0, 0, 1, 
        1, 1, 0, 0, 
        2, 0, 0, 0, 
        1, 0, 0, 0, 
        1
    }, //2, hash

    {
        0, 0, 0, 0, 
        1, 0, 0, 0, 
        1, 0, 0, 0, 
        0, 0, 0, 0, 
        1
    }, //3, check

    {
        0, 0, 0, 0, 
        1, 0, 0, 0, 
        1, 0, 1, 0, 
        0, 0, 1, 0, 
        2
    }, //4, adapt
};

INSTANTIATE_TEST_CASE_P(
	IBCH_Test,
    IB_CH_KEF_CZS_2014_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_KEF_CZS_2014_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        IB_CH_KEF_CZS_2014 ch(GetParam().curve, GetParam().swap);

        IB_CH_KEF_CZS_2014_pp pp;
        IB_CH_KEF_CZS_2014_msk msk;
        IB_CH_KEF_CZS_2014_td td;
        IB_CH_KEF_CZS_2014_h h;
        IB_CH_KEF_CZS_2014_r r,r_p;

        std::string ID = "identity string ID";
        std::string L = "customozed identity L";

        element_s *m = ch.GetZrElement();
        element_s *m_p = ch.GetZrElement();

        this->start("SetUp");
        ch.SetUp(pp, msk);
        this->end("SetUp");
        if(visiable){
            pp.print();
        }

        this->start("Extract");
        ch.Extract(td, ID, msk);
        this->end("Extract");
        if(visiable){
            td.print();
        }
    
        this->start("Hash");
        ch.Hash(h, r, ID, L, m, pp);
        this->end("Hash");
        if(visiable){
            Logger::PrintPbc("m", m);
            h.print();
            r.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, L, m, td);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, r, L, m, td);
        this->end("Adapt");
        if(visiable){    
            Logger::PrintPbc("m_p", m_p);
            r_p.print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, r_p, L, m_p, td);
        this->end("Verify");
        ASSERT_TRUE(verify_result);
    }
    average();
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[0], "SetUp"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[1], "Extract"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[2], "Hash"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Check"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[4], "Adapt"));
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[3], "Verify"));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



