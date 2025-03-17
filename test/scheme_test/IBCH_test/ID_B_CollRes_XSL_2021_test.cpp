#include "scheme/IBCH/ID_B_CollRes_XSL_2021.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int length;
};

class ID_B_CollRes_XSL_2021_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap:%d length:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().length);
            if(visiable)printf("%s %s swap:%d length:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().length);
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

    int lengths[] = {64, 128, 256};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for(int len: lengths){
                test_params.push_back({curve, swap, len});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	IBCH_Test,
    ID_B_CollRes_XSL_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(ID_B_CollRes_XSL_2021_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        ID_B_CollRes_XSL_2021 ch(GetParam().curve, GetParam().swap);

        ID_B_CollRes_XSL_2021_pp pp;
        ID_B_CollRes_XSL_2021_msk msk;
        ID_B_CollRes_XSL_2021_tk tk;
        ID_B_CollRes_XSL_2021_h h;
        ID_B_CollRes_XSL_2021_r r,r_p;

        std::string I = "IDENTITYIDENTITYIDENTITYIDENTITY";
        
        element_s *m = ch.GetZrElement();
        element_s *m_p = ch.GetZrElement();    

        this->start("SetUp");
        ch.SetUp(pp, msk, GetParam().length);
        this->end("SetUp");
        if(visiable){
            pp.get_pp().print();
            msk.print();
        }

        this->start("KeyGen");
        ch.KeyGen(tk, msk, I.c_str(), pp);
        this->end("KeyGen");
        if(visiable){
            tk.print();
        }
        
        this->start("Hash");
        ch.Hash(h, r, m, I.c_str(), pp);
        this->end("Hash");
        if(visiable){
            Logger::PrintPbc("m", m);
            h.print();
            r.print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, m, r, I.c_str(), pp);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, m, r, tk);
        this->end("Adapt");
        if(visiable){
            Logger::PrintPbc("m_p", m_p);
            h.print();
            r_p.print();
        }
        
        this->start("Verify");
        bool verify_result = ch.Verify(h, m_p, r_p, I.c_str(), pp);
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



