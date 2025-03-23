#include "scheme/IBCH/ID_B_CollRes_XSL_2021.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int length;
};

std::ostream& operator<<(std::ostream& os, const TestParams& params) {
    return os << "curve=" << Curve::curve_names[params.curve] << " swap=" << params.swap << " length=" << params.length;
}

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

int op_cnt[][diff_max_len] = {
    {
        1, 1, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 1, 0, 0, 
        0
    }, //0, setup

    {
        0, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //1, KeyGen
    
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

TEST_P(ID_B_CollRes_XSL_2021_Test, Test){
    ID_B_CollRes_XSL_2021 ch(GetParam().curve, GetParam().swap);

    ID_B_CollRes_XSL_2021_pp pp[repeat];
    ID_B_CollRes_XSL_2021_msk msk[repeat];
    ID_B_CollRes_XSL_2021_tk tk[repeat];
    ID_B_CollRes_XSL_2021_h h[repeat];
    ID_B_CollRes_XSL_2021_r r[repeat], r_p[repeat];

    std::string I = "IDENTITYIDENTITYIDENTITYIDENTITY";

    element_s *m[repeat], *m_p[repeat];
    for (int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pp[i], msk[i], GetParam().length);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(tk[i], msk[i], I.c_str(), pp[i]);
    this->end("KeyGen");

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m[i], I.c_str(), pp[i]);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h[i], m[i], r[i], I.c_str(), pp[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h[i], m[i], r[i], tk[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], m_p[i], r_p[i], I.c_str(), pp[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);

    average();

    int op_cnt_SetUp[diff_max_len];
    for(int i=0; i<diff_max_len; i++) op_cnt_SetUp[i] = op_cnt[0][i];
    int delta_SetUp[] = {
        0, 1, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0
    };
    for(int j=0;j<GetParam().length;j++){
        for(int i=0;i<diff_max_len;i++) op_cnt_SetUp[i] += delta_SetUp[i];
    }
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_SetUp, "SetUp"));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



