#include "scheme/CH/CH_KEF_DL_CZT_2011.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class CH_KEF_DL_CZT_2011_Test : public BaseTest<TestParams>{    
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            std::string groupName;
            if (GetParam().group == Group::G1) {
                groupName = "G1";
            } else if (GetParam().group == Group::G2) {
                groupName = "G2";
            } else if (GetParam().group == Group::GT) {
                groupName = "GT";
            } else {
                groupName = "UNKNOWN";
            }
            fprintf(out, "%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
            if(visiable)printf("%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
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

    int groups[] = {Group::G1, Group::G2, Group::GT};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int group : groups) {
            test_params.push_back({curve, group});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_DL_CZT_2011_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_DL_CZT_2011_Test, Test){
    CH_KEF_DL_CZT_2011 ch(GetParam().curve, GetParam().group);
    
    CH_KEF_DL_CZT_2011_pp pp;
    CH_KEF_DL_CZT_2011_pk pk;
    CH_KEF_DL_CZT_2011_sk sk;
    CH_KEF_DL_CZT_2011_r r, r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *I = ch.GetZrElement();
    element_s *h = ch.GetG1Element();


    this->start("SetUp");
    ch.SetUp(pp, pk, sk, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, I, m, pk, pp);
    this->end("Hash");
    if(visiable){
        Logger::PrintPbc("m", m);
        Logger::PrintPbc("I", I);
        r.print();
    }

    this->start("Check");
    bool check_result = ch.Check(h, r, I, m, pk, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    
    this->start("Adapt");
    ch.Adapt(r_p, sk, h, m, r, m_p, I, pp);
    this->end("Adapt");
    if(visiable){
        Logger::PrintPbc("m_p", m_p);
        r_p.print();
    }

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, I, m_p, pk, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






