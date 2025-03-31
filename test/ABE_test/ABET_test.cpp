#include "ChameLib.h"
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

    const std::string POLICY = "A&(DDDD|(BB&CCC))";
    const int SIZE_OF_POLICY = 4;
    // compute MSP
    Policy_resolution pr;
    Policy_generation pg;
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    std::vector<std::string> S1 = {"A","DDDD"};
    const int SIZE_OF_S1 = S1.size();

    std::vector<std::string> S2 = {"BB","CCC"};
    const int SIZE_OF_S2 = S2.size();

    int K = GetParam().k;
    const int U1 = K/3;  // length of U1
    const int U2 = K/2;  // length of U2
    ABET_ID id_12;
    id_12.init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = abe.GetZrElement();
        id_12.set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }
    
    ABET_mpk mpk[repeat];
    ABET_msk msk[repeat];
    ABET_sks sks_1[repeat];
    ABET_sks sks_2[repeat];

    ABET_ciphertext ciphertext_1[repeat];
    ABET_ciphertext ciphertext_2[repeat];

    // r
    element_s *r[repeat], *res_r[repeat];
    for (int i = 0; i < repeat; i++){
        r[i] = abe.GetZrElement();
        res_r[i] = abe.GetZrElement();
    }
    
    int len_r_half = element_length_in_bytes(r[0]) / 2;
    // R
    unsigned char R[repeat][len_r_half];
    unsigned char res_R[repeat][len_r_half];
    for (int i = 0; i < repeat; i++) RandomGenerator::Random_bytes(R[i], len_r_half);

    element_s *s1[repeat], *s2[repeat];
    for (int i = 0; i < repeat; i++){
        s1[i] = abe.GetZrElement();
        s2[i] = abe.GetZrElement();
    }

    this->start("Setup");
    for (int i = 0; i < repeat; i++) abe.Setup(msk[i], mpk[i], K);
    this->end("Setup");

    // U1
    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) abe.KeyGen(sks_1[i], msk[i], mpk[i], S1, id_12, U1);
    this->end("KeyGen");
    // U2
    for (int i = 0; i < repeat; i++) abe.KeyGen(sks_2[i], msk[i], mpk[i], S2, id_12, U2);

    this->start("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Encrypt(ciphertext_1[i], mpk[i], msk[i], r[i], R[i], element_length_in_bytes(r[i]) / 2, MSP, id_12, U1, s1[i], s2[i]);
    this->end("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Encrypt(ciphertext_2[i], mpk[i], msk[i], r[i], R[i], element_length_in_bytes(r[i]) / 2, MSP, id_12, U2, s1[i], s2[i]);

    this->start("Decrypt");
    for (int i = 0; i < repeat; i++) abe.Decrypt(res_R[i], res_r[i], mpk[i], msk[i], ciphertext_1[i], sks_1[i], MSP, id_12, U1);
    this->end("Decrypt");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(memcmp(R[i], res_R[i], element_length_in_bytes(r[i]) / 2) == 0);
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(element_cmp(r[i], res_r[i]) == 0);
    for (int i = 0; i < repeat; i++){
        try{
            abe.Decrypt(res_R[i], res_r[i], mpk[i], msk[i], ciphertext_2[i], sks_2[i], MSP, id_12, U2);
        }catch(const std::runtime_error& e){}

        element_random(res_r[i]);
        RandomGenerator::Random_bytes(res_R[i], element_length_in_bytes(r[0]) / 2);
        abe.Decrypt(res_R[i], res_r[i], mpk[i], msk[i], ciphertext_2[i], sks_1[i], MSP, id_12, U1);
        ASSERT_TRUE(memcmp(R[i], res_R[i], element_length_in_bytes(r[i]) / 2) == 0);    
        ASSERT_TRUE(element_cmp(r[i], res_r[i]) == 0);
    }

    average();
}

int main(int argc, char **argv) 
{
    if (argc > 1) {
        repeat = std::atoi(argv[1]);
        if (repeat <= 0) {
            std::cerr << "Invalid value for repeat. It must be a positive integer." << std::endl;
            return 1;
        }
    }

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}