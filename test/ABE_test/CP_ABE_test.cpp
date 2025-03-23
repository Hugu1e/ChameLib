#include "ABE/CP_ABE.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
};

class CP_ABE_Test : public BaseTest<TestParams>{    
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            if(visiable)printf("%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
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

INSTANTIATE_TEST_CASE_P(
	ABE_Test,
    CP_ABE_Test,
	testing::ValuesIn(test_values)
);

int op_cnt[][diff_max_len] = {
    {
        1, 1, 0, 7, 
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        3, 2, 2, 0, 
        1
    }, //0, setup

    {
        0, 0, 0, 3, 
        6, 0, 0, 0, 
        9, 0, 0, 10, 
        9, 3, 0, 0, 
        0
    }, //1, keygen
    
    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 2, 0, 
        0, 3, 2, 0, 
        0
    }, //2, Encrypt

    {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        3, 0, 6, 0, 
        0, 0, 0, 0, 
        6
    }, //3, Decrypt
};

TEST_P(CP_ABE_Test, Test){
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    
    CP_ABE abe(GetParam().curve, GetParam().swap);
    CP_ABE_mpk mpk[repeat];
    CP_ABE_msk msk[repeat];
    CP_ABE_sks sks[repeat];
    CP_ABE_ciphertext ciphertext[repeat];

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    Policy_resolution pr;
    Policy_generation pg;
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);

    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    int rows = MSP->row();
    int cols = MSP->col();

    element_s *msg[repeat], *res[repeat];
    for (int i = 0; i < repeat; i++){
        msg[i] = abe.GetGTElement();
        res[i] = abe.GetGTElement();
    }
    
    this->start("SetUp");
    for (int i = 0; i < repeat; i++) abe.Setup(msk[i], mpk[i]);
    this->end("SetUp");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) abe.KeyGen(sks[i], msk[i], mpk[i], attr_list);
    this->end("KeyGen");

    element_s *s1[repeat], *s2[repeat];
    for (int i = 0; i < repeat; i++){
        s1[i] = abe.GetZrElement();
        s2[i] = abe.GetZrElement();
    }
    this->start("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Encrypt(ciphertext[i], mpk[i], msg[i], MSP, s1[i], s2[i]);
    this->end("Encrypt");

    this->start("Decrypt");
    for (int i = 0; i < repeat; i++) abe.Decrypt(res[i], ciphertext[i], MSP, mpk[i], sks[i]);
    this->end("Decrypt");

    for (int i = 0; i < repeat; i++) ASSERT_TRUE(element_cmp(msg[i], res[i]) == 0);
    
    average();

    EXPECT_TRUE(check_time(GetParam().curve, op_cnt[0], "SetUp"));

    int op_cnt_KeyGen[diff_max_len];
    for(int i=0; i<diff_max_len; i++) op_cnt_KeyGen[i] = op_cnt[1][i];
    for(int i = 0;i < attr_list.size();i++){
        int delta[] = {
            0, 0, 0, 1, 
            6, 0, 0, 0, 
            6, 0, 0, 2, 
            9, 0, 0, 0, 
            0
        };
        for(int j=0; j<diff_max_len; j++){
            op_cnt_KeyGen[j] += delta[j];
        }
    }
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

    int op_cnt_Encrypt[diff_max_len];
    for(int i=0; i<diff_max_len; i++) op_cnt_Encrypt[i] = op_cnt[2][i];
    for(int i = 0; i < rows; i++){
        int delta_row[] = {
            0, 0, 0, 0, 
            6, 0, 0, 0, 
            3, 0, 0, 0, 
            6, 0, 0, 0, 
            0
        };
        for(int j=0; j<diff_max_len; j++){
            op_cnt_Encrypt[j] += delta_row[j];
        }
        int delta_col[] = {
            0, 0, 0, 0, 
            2, 0, 0, 0, 
            2, 0, 0, 0, 
            3, 0, 0, 0, 
            0
        };
        for(int l=0; l<cols; l++){
            for(int j=0; j<diff_max_len; j++){
                op_cnt_Encrypt[j] += delta_col[j];
            }
        }
    }
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Encrypt, "Encrypt"));

    int op_cnt_Decrypt[diff_max_len];
    for(int i=0; i<diff_max_len; i++) op_cnt_Decrypt[i] = op_cnt[3][i];
    int delta_num_den[] = {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        6, 0, 0, 0, 
        6, 0, 0, 0, 
        0
    };
    for(unsigned long int i=0; i<rows;i++){
        if(sks[0].get_attr2id().find(MSP->getName(i)) == sks[0].get_attr2id().end()){
            for(int j=0; j<diff_max_len; j++){
                op_cnt_Decrypt[j] += delta_num_den[j];
            }
        }
    }
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Decrypt, "Decrypt"));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}