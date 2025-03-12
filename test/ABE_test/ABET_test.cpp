#include "ABE/ABET.h"
#include "CommonTest.h"
#include "utils/RandomGenerator.h"

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

    const std::string POLICY = "A&(DDDD|(BB&CCC))";
    const int SIZE_OF_POLICY = 4;

    std::vector<std::string> S1 = {"A","DDDD"};
    const int SIZE_OF_S1 = S1.size();

    std::vector<std::string> S2 = {"BB","CCC"};
    const int SIZE_OF_S2 = S2.size();

    int K = GetParam().k;
    const int U1 = K/3;  // length of U1
    const int U2 = K/2;  // length of U2

    ABET_ID id;
    ABET_mpk mpk;
    ABET_msk msk;
    ABET_sks sks_1;
    ABET_sks sks_2;

    ABET_ciphertext ciphertext_1;
    ABET_ciphertext ciphertext_2;

    id.init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = abe.GetZrElement();
        id.set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }
    // if(visiable) id.print();

    // r
    element_s *r = abe.GetZrElement();
    element_s *res_r = abe.GetZrElement();
    // R
    unsigned char R[element_length_in_bytes(r) / 2];
    unsigned char res_R[element_length_in_bytes(r) / 2];
    RandomGenerator::Random_bytes(R, element_length_in_bytes(r) / 2);
    if(visiable){
        Logger::PrintPbc("r", r);
        printf("R: %s\n", R);        
    }

    element_s *s1 = abe.GetZrElement();
    element_s *s2 = abe.GetZrElement();

    this->start("Setup");
    abe.Setup(msk, mpk, K);
    this->end("Setup");

    // U1
    this->start("KeyGen");
    abe.KeyGen(sks_1, msk, mpk, S1, id, U1);
    this->end("KeyGen");
    abe.KeyGen(sks_2, msk, mpk, S2, id, U2);

    this->start("Encrypt");
    abe.Encrypt(ciphertext_1, mpk, msk, r, R, element_length_in_bytes(r) / 2, POLICY, id, U1, s1, s2);
    this->end("Encrypt");
    abe.Encrypt(ciphertext_2, mpk, msk, r, R, element_length_in_bytes(r) / 2, POLICY, id, U2, s1, s2);

    this->start("Decrypt");
    abe.Decrypt(res_R, res_r, mpk, msk, ciphertext_1, sks_1, POLICY, id, U1, U1);
    this->end("Decrypt");
    ASSERT_TRUE(memcmp(R, res_R, element_length_in_bytes(r) / 2) == 0);
    ASSERT_TRUE(element_cmp(r, res_r) == 0);
    if(visiable){
        Logger::PrintPbc("res_r", res_r);
        printf("res_R_1: %s\n", res_R);
    }

    try{
        abe.Decrypt(res_R, res_r, mpk, msk, ciphertext_2, sks_2, POLICY, id, U2, U2);
    }catch(const std::runtime_error& e){
        if(visiable) printf("%s\n", e.what());
    }

    element_random(res_r);
    RandomGenerator::Random_bytes(R, element_length_in_bytes(r) / 2);
    abe.Decrypt(res_R, res_r, mpk, msk, ciphertext_2, sks_1, POLICY, id, U1, U2);
    if(visiable){
        Logger::PrintPbc("res_r", res_r);
        printf("res_R_2: %s\n", res_R);
    }
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}