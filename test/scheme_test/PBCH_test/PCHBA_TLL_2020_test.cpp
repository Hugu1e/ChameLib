#include "scheme/PBCH/PCHBA_TLL_2020.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCHBA_TLL_2020_Test : public BaseTest<TestParams>{
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

    int ks[] = {8, 16, 24};

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
    PCHBA_TLL_2020_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCHBA_TLL_2020_Test, Test){
    PCHBA_TLL_2020 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;  

    const int K = 10;
    const int I = 5;  // modifier
    const int J = 5;  // owner
    PCHBA_TLL_2020_sk skPCHBA;
    PCHBA_TLL_2020_pk pkPCHBA;
    PCHBA_TLL_2020_ID ID;
    PCHBA_TLL_2020_sks sksPCHBA;
    PCHBA_TLL_2020_h h, h_p;

    ID.get_IDABET().init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = ch.GetZrElement();

        ID.get_IDABET().set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }
    if(visiable) ID.get_IDABET().print();

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pkPCHBA, skPCHBA, sksPCHBA, h, h_p, K);
    this->end("SetUp");
    
    this->start("KeyGen");
    ch.KeyGen(sksPCHBA, pkPCHBA, skPCHBA, attr_list, ID, I);
    this->end("KeyGen");
    
    this->start("Hash");
    ch.Hash(h, m, pkPCHBA, skPCHBA, POLICY, ID, J);
    this->end("Hash");
    
    this->start("Check");
    bool check_result = ch.Check(h, m, pkPCHBA);
    this->end("Check");
    ASSERT_TRUE(check_result);
    
    this->start("Adapt");
    ch.Adapt(h_p, m_p, h, m, POLICY, ID, I, pkPCHBA, skPCHBA, sksPCHBA);
    this->end("Adapt");

    this->start("Verify");
    bool verify_result = ch.Verify(h_p, m_p, pkPCHBA);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
    
    this->start("Judge");
    bool judgeRes = ch.Judge(m, h, m_p, h_p, ID, I, pkPCHBA, skPCHBA);
    this->end("Judge");
    
    ASSERT_TRUE(judgeRes);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}