#include "ABE/ABET.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class ABET_Test : public BaseTest<TestParams>{
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
	ABE_Test,
    ABET_Test,
	testing::ValuesIn(test_values)
);

TEST_P(ABET_Test, Test){
    ABET abe(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE","FOUR"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    const int K = 10;
    const int I = 5;  // modifier
    const int J = 5;  // owner
    ABET_ID id;
    ABET_mpk mpk;
    ABET_msk msk;
    ABET_sks sks;

    ABET_ciphertext ciphertext;

    id.init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = abe.GetZrElement();
        id.set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }
    if(visiable) id.print();

    element_s *r = abe.GetZrElement();
    element_s *R = abe.GetZrElement();
    element_s *res_r = abe.GetZrElement();
    element_s *res_R = abe.GetZrElement();
    element_s *s1 = abe.GetZrElement();
    element_s *s2 = abe.GetZrElement();

    this->start("Setup");
    abe.Setup(msk, mpk, sks, ciphertext, K);
    this->end("Setup");

    this->start("KeyGen");
    abe.KeyGen(sks, msk, mpk, attr_list, id, I);
    this->end("KeyGen");

    if(visiable){
        Logger::PrintPbc("R", R);
        Logger::PrintPbc("r", r);        
    }

    this->start("Encrypt");
    abe.Encrypt(ciphertext, mpk, msk, r, R, POLICY, id, J, s1, s2);
    this->end("Encrypt");
   
    this->start("Decrypt");
    abe.Decrypt(res_R, res_r, mpk, ciphertext, sks);
    this->end("Decrypt");
    if(visiable){
        Logger::PrintPbc("res_R", res_R);
        Logger::PrintPbc("res_r", res_r);
    }

    ASSERT_TRUE(element_cmp(R,res_R) == 0);
    ASSERT_TRUE(element_cmp(r,res_r) == 0);
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}