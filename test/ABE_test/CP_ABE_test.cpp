#include "ChameLib.h"
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

TEST_P(CP_ABE_Test, Test){
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    
    CP_ABE abe(GetParam().curve, GetParam().swap);
    CP_ABE_mpk mpk[repeat];
    CP_ABE_msk msk[repeat];
    CP_ABE_sks sks[repeat];
    CP_ABE_ciphertext ciphertext[repeat];

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, abe.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

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

    this->start("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Encrypt(ciphertext[i], mpk[i], msg[i], MSP);
    this->end("Encrypt");

    this->start("Decrypt");
    for (int i = 0; i < repeat; i++) abe.Decrypt(res[i], ciphertext[i], MSP, mpk[i], sks[i]);
    this->end("Decrypt");

    for (int i = 0; i < repeat; i++) ASSERT_TRUE(element_cmp(msg[i], res[i]) == 0);
    
    average();

    if(!GetParam().swap){
        int op_cnt_SetUp[] = {
            1, 1, 0, 7, 
            0, 0, 0, 0, 
            0, 0, 0, 2, 
            3, 2, 2, 0, 
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_SetUp, "SetUp"));

        int n = attr_list.size();
        int op_cnt_KeyGen[] = {
            0, 0, 0, 3 + n, 
            6 + 6*n, 0, 0, 0, 
            9 + 6*n, 0, 0, 10 + 2*n, 
            9 + 9*n, 3, 0, 0, 
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

        int op_cnt_Encrypt[] = {
            0, 0, 0, 2, 
            6*rows + 6*rows*cols, 0, 0, 0, 
            3*rows + 6*rows*cols, 0, 2, 0, 
            6*rows + 9*rows*cols, 3, 2, 0, 
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Encrypt, "Encrypt"));

        int op_cnt_Decrypt[] = {
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            0, 0, 6, 0, 
            0, 0, 0, 0, 
            6
        };
        int delta_num_den[] = {
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            6, 0, 0, 0, 
            6, 0, 0, 0, 
            0
        };
        for(unsigned long int i=0; i<rows;i++){
            if(sks[0].get_attr2id().find(MSP->getName(i)) != sks[0].get_attr2id().end()){
                for(int j=0; j<diff_max_len; j++){
                    op_cnt_Decrypt[j] += delta_num_den[j];
                }
            }
        }
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Decrypt, "Decrypt"));
    }else{
        int op_cnt_SetUp[] = {
            1, 1, 0, 7, 
            0, 0, 0, 0, 
            0, 0, 0, 2, 
            2, 3, 2, 0, 
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_SetUp, "SetUp"));

        int n = attr_list.size();
        int op_cnt_KeyGen[] = {
            0, 0, 0, 3 + n, 
            0, 6 + 6*n, 0, 0, 
            0, 9 + 6*n, 0, 10 + 2*n, 
            3, 9 + 9*n, 0, 0, 
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

        int op_cnt_Encrypt[] = {
            0, 0, 0, 2, 
            0, 6*rows + 6*rows*cols, 0, 0, 
            0, 3*rows + 6*rows*cols, 2, 0, 
            3, 6*rows + 9*rows*cols, 2, 0, 
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Encrypt, "Encrypt"));

        int op_cnt_Decrypt[] = {
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            0, 0, 6, 0, 
            0, 0, 0, 0, 
            6
        };
        int delta_num_den[] = {
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            0, 6, 0, 0, 
            0, 6, 0, 0, 
            0
        };
        for(unsigned long int i=0; i<rows;i++){
            if(sks[0].get_attr2id().find(MSP->getName(i)) != sks[0].get_attr2id().end()){
                for(int j=0; j<diff_max_len; j++){
                    op_cnt_Decrypt[j] += delta_num_den[j];
                }
            }
        }
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Decrypt, "Decrypt"));
    }
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}