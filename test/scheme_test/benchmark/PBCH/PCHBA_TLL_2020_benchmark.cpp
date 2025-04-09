#include "ChameLib.h"
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

    const std::string POLICY = "A&(DDDD|(BB&CCC))";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    std::vector<std::string> S1 = {"A","DDDD"};
    const int SIZE_OF_S1 = S1.size();

    std::vector<std::string> S2 = {"BB","CCC"};
    const int SIZE_OF_S2 = S2.size();

    int K = GetParam().k;
    const int U1 = K/3;  // length of U1
    const int U2 = K/2;  // length of U2
    PCHBA_TLL_2020_ID ID12;
    ID12.get_IDABET().init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = ch.GetZrElement();

        ID12.get_IDABET().set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }

    PCHBA_TLL_2020_sk skPCHBA[repeat];
    PCHBA_TLL_2020_pk pkPCHBA[repeat];
    
    PCHBA_TLL_2020_sks sksPCHBA_1[repeat];
    PCHBA_TLL_2020_h h2[repeat];
    PCHBA_TLL_2020_r r2[repeat], r_p[repeat];

    element_s *m2[repeat], *m_p[repeat];
    for (int i = 0; i < repeat; i++){
        m2[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
    }

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(pkPCHBA[i], skPCHBA[i], K);
    this->end("SetUp");
    
    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) ch.KeyGen(sksPCHBA_1[i], pkPCHBA[i], skPCHBA[i], S1, ID12, U1);
    this->end("KeyGen");
    
    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h2[i], r2[i], m2[i], pkPCHBA[i], skPCHBA[i], MSP, ID12, U2);
    this->end("Hash");
    
    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(h2[i], r2[i], m2[i], pkPCHBA[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p[i], h2[i], r2[i], m2[i], MSP, ID12, U1, pkPCHBA[i], skPCHBA[i], sksPCHBA_1[i]);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h2[i], r_p[i], m_p[i], pkPCHBA[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
    average();

    if(!GetParam().swap){
        int k = GetParam().k;
        int op_cnt_SetUp[] = {
            1, 1, 0, 1 + 6 + 3 + k, 
            0, 0, 0, 0, 
            0, 0, 0, 1 + 2 + 3, 
            4 + 2 * k, 6 + k, 2, 0, 
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_SetUp, "SetUp"));
    
        int n = S1.size();
        int ID_len = U1;
        int op_cnt_KeyGen[] = {    
            0, 0, 0, 3 + n + 1, 
            6 + 6 * n, 0, 0, 0, 
            6 * n + ID_len + 13, 0, 0, 17 + 2 * n,
            16 + 9 * n + k, 3, 0, 0,
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));
    
        n = MSP->row();
        int m = MSP->col();
        int op_cnt_Hash[] = {
            0, 0, 0, 1 + 2 + 2 + 1,
            3 * n * (2 * m + 2), 0, 0, 2 + 1,
            3 * n * (2 * m + 1), 1 + ID_len, 1, 1,
            3 * n * (3 * m + 2), 11 + ID_len, 3, 0,
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Hash, "Hash"));
    
        int op_cnt_Check[] = {    
            0, 0, 0, 0, 
            0, 0, 0, 1, 
            0, 1, 1, 0, 
            0, 1, 2, 0, 
            3
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Check, "Check"));
    
        int Owner_ID_len = U1;
        int Modifier_ID_len = U1;
        int delta_len = Owner_ID_len - Modifier_ID_len;
        int op_cnt_Adapt_1[] = {    
            0, 0, 0, 2 * delta_len, 
            (6 * n + 6) * delta_len, 0, 0, 1, 
            (1 + 6 * n + 6 + 2 + k) * delta_len - (Owner_ID_len + Modifier_ID_len + 1) * delta_len / 2, 1 + 3 * delta_len, 1, 10 * delta_len, 
            (1 + 6 * n + 6 + 2 + k) * delta_len - (Owner_ID_len + Modifier_ID_len + 1) * delta_len / 2, 1 + 3 * delta_len, 2, 0, 
            3
        };
        int op_cnt_Adapt_2[] = {    
            0, 0, 0, 1 + 2 + 2, 
            3 * n * (2 * m + 2), 0, 0, 1 + 2, 
            6 * n + ID_len + 3 * n * (2 * m + 1), 0, 2 + 5 + 1, 2 + 1, 
            6 * n + ID_len + 3 * n * (3 * m + 2), 1 + 4 + 3 + 1, 2 + 1, 0, 
            3 + 6 + 1
        };
        for(int i=0; i<diff_max_len;i++){
            op_cnt_Adapt_1[i] += op_cnt_Adapt_2[i];
        }
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Adapt_1, "Adapt"));
    
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Check, "Verify"));
    }else{
        int k = GetParam().k;
        int op_cnt_SetUp[] = {
            1, 1, 0, 1 + 6 + 3 + k, 
            0, 0, 0, 0, 
            0, 0, 0, 1 + 2 + 3, 
            6 + k, 4 + 2 * k, 2, 0, 
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_SetUp, "SetUp"));

        int n = S1.size();
        int ID_len = U1;
        int op_cnt_KeyGen[] = {    
            0, 0, 0, 3 + n + 1, 
            0, 6 + 6 * n, 0, 0, 
            0, 6 * n + ID_len + 13, 0, 17 + 2 * n,
            3, 16 + 9 * n + k, 0, 0,
            0
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

        n = MSP->row();
        int m = MSP->col();
        int op_cnt_Hash[] = {
            0, 0, 0, 1 + 2 + 2 + 1,
            0, 3 * n * (2 * m + 2), 0, 2 + 1,
            1 + ID_len, 3 * n * (2 * m + 1), 1, 1,
            11 + ID_len, 3 * n * (3 * m + 2), 3, 0,
            1
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Hash, "Hash"));

        int op_cnt_Check[] = {    
            0, 0, 0, 0, 
            0, 0, 0, 1, 
            1, 0, 1, 0, 
            1, 0, 2, 0, 
            3
        };
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Check, "Check"));

        int Owner_ID_len = U1;
        int Modifier_ID_len = U1;
        int delta_len = Owner_ID_len - Modifier_ID_len;
        int op_cnt_Adapt_1[] = {    
            0, 0, 0, 2 * delta_len, 
            0, (6 * n + 6) * delta_len, 0, 1, 
            1 + 3 * delta_len, (1 + 6 * n + 6 + 2 + k) * delta_len - (Owner_ID_len + Modifier_ID_len + 1) * delta_len / 2, 1, 10 * delta_len, 
            1 + 3 * delta_len, (1 + 6 * n + 6 + 2 + k) * delta_len - (Owner_ID_len + Modifier_ID_len + 1) * delta_len / 2, 2, 0, 
            3
        };
        int op_cnt_Adapt_2[] = {    
            0, 0, 0, 1 + 2 + 2, 
            0, 3 * n * (2 * m + 2), 0, 1 + 2, 
            0, 6 * n + ID_len + 3 * n * (2 * m + 1), 2 + 5 + 1, 2 + 1, 
            1 + 4 + 3 + 1, 6 * n + ID_len + 3 * n * (3 * m + 2), 2 + 1, 0, 
            3 + 6 + 1
        };
        for(int i=0; i<diff_max_len;i++){
            op_cnt_Adapt_1[i] += op_cnt_Adapt_2[i];
        }
        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Adapt_1, "Adapt"));

        EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Check, "Verify"));
    }

    // free
    delete binary_tree_expression;
    delete MSP;
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}