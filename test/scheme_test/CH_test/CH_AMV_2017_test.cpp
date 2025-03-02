#include "scheme/CH/CH_AMV_2017.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int group;
};

class CH_AMV_2017_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            visiable = true;

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
    CH_AMV_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_AMV_2017_Test, Test){
    CH_AMV_2017 ch = CH_AMV_2017(GetParam().curve, GetParam().group);

    CH_AMV_2017_pk pk;
    CH_AMV_2017_sk sk;
    CH_AMV_2017_h h,h_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pk, sk, h, h_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk);
    this->end("KeyGen");
    if(visiable){
        Logger::PrintPbc("g", pk.get_CH_pk()[CH_AMV_2017::g]);
        Logger::PrintPbc("y", pk.get_CH_pk()[CH_AMV_2017::y]);
        Logger::PrintPbc("x", sk.get_CH_sk()[CH_AMV_2017::x]);        
    }
    
    this->start("Hash");
    ch.Hash(h, m, pk);
    this->end("Hash");
    if(visiable){
        Logger::PrintPbc("m", m);
        Logger::PrintPbc("Hash value", h.get_h()[CH_AMV_2017::h1]);
    }

    this->start("Check");
    bool check_result = ch.Check(h, m, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("Adapt");
    ch.Adapt(h_p, m_p, h, m, sk, pk);
    this->end("Adapt");
    if(visiable) Logger::PrintPbc("m_p", m_p);

    this->start("Verify");
    bool verify_result = ch.Verify(h_p, m_p, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}