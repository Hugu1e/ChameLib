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

            repeat = 10;

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
        11, 3, 0, 0, 
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
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0
    }, //3, Decrypt
};

TEST_P(CP_ABE_Test, Test){
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    int rows, cols;  // MSP
    
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        CP_ABE abe(GetParam().curve, GetParam().swap);
        CP_ABE_mpk mpk;
        CP_ABE_msk msk;
        CP_ABE_sks sks;
        CP_ABE_ciphertext ciphertext;

        
        const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
        const int SIZE_OF_POLICY = 4;

        // compute MSP
        Policy_resolution pr;
        Policy_generation pg;
        std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
        if(visiable){
            printf("postfix_expression of Policy: ");
            for(int i = 0;i < postfix_expression->size();i++){
                printf("%s ", postfix_expression->at(i).c_str());
            }
            printf("\n");
        }
        Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
        pg.generatePolicyInMatrixForm(binary_tree_expression);
        Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
        if(visiable){
            printf("Policy Matrix:\n");
            MSP->printMatrix();
        }
        rows = MSP->row();
        cols = MSP->col();

        element_s* msg = abe.GetGTElement();
        element_s* res = abe.GetGTElement();
        
        this->start("SetUp");
        abe.Setup(msk, mpk);
        this->end("SetUp");

        this->start("KeyGen");
        abe.KeyGen(sks, msk, mpk, attr_list);
        this->end("KeyGen");

        if(visiable){
            Logger::PrintPbcWithSize("msg", msg);
        }
        element_s *s1 = abe.GetZrElement();
        element_s *s2 = abe.GetZrElement();
        this->start("Encrypt");
        abe.Encrypt(ciphertext, mpk, msg, MSP, s1, s2);
        this->end("Encrypt");

        this->start("Decrypt");
        abe.Decrypt(res, ciphertext, MSP, mpk, sks);
        this->end("Decrypt");
        if(visiable){
            Logger::PrintPbc("res", res);        
        }
        ASSERT_TRUE(element_cmp(msg, res) == 0);
    }
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
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}