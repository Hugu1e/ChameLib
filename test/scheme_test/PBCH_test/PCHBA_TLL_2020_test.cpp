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
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        PCHBA_TLL_2020 ch(GetParam().curve, GetParam().swap);

        const std::string POLICY = "A&(DDDD|(BB&CCC))";
        const int SIZE_OF_POLICY = 4;

        // compute policy matrix
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
        Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, ch.GetZrElement());
        pg.generatePolicyInMatrixForm(binary_tree_expression);
        Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
        if(visiable){
            printf("Policy Matrix:\n");
            MSP->printMatrix();
        }

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
        // if(visiable) ID12.get_IDABET().print();

        PCHBA_TLL_2020_sk skPCHBA;
        PCHBA_TLL_2020_pk pkPCHBA;
        
        PCHBA_TLL_2020_sks sksPCHBA_1, sksPCHBA_2;
        PCHBA_TLL_2020_h h1, h2;
        PCHBA_TLL_2020_r r1, r2, r_p;

        element_s *m1 = ch.GetZrElement();
        element_s *m2 = ch.GetZrElement();
        element_s *m_p = ch.GetZrElement();

        this->start("SetUp");
        ch.SetUp(pkPCHBA, skPCHBA, K);
        this->end("SetUp");
        
        this->start("KeyGen");
        ch.KeyGen(sksPCHBA_1, pkPCHBA, skPCHBA, S1, ID12, U1);
        this->end("KeyGen");
        ch.KeyGen(sksPCHBA_2, pkPCHBA, skPCHBA, S2, ID12, U2);
        
        this->start("Hash");
        ch.Hash(h1, r1, m1, pkPCHBA, skPCHBA, MSP, ID12, U1);
        this->end("Hash");
        
        this->start("Check");
        bool check_result = ch.Check(h1, r1, m1, pkPCHBA);
        this->end("Check");
        ASSERT_TRUE(check_result);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h1, r1, m1, MSP, ID12, U1, pkPCHBA, skPCHBA, sksPCHBA_1);
        this->end("Adapt");

        this->start("Verify");
        bool verify_result = ch.Verify(h1, r_p, m_p, pkPCHBA);
        this->end("Verify");
        ASSERT_TRUE(verify_result);

        ch.Hash(h2, r2, m2, pkPCHBA, skPCHBA, MSP, ID12, U2);
        check_result = ch.Check(h2, r2, m2, pkPCHBA);
        ASSERT_TRUE(check_result);
        try{
            ch.Adapt(r_p, m_p, h2, r2, m2, MSP, ID12, U2, pkPCHBA, skPCHBA, sksPCHBA_2);
        }catch(const std::runtime_error& e){
            if(visiable) printf("%s\n", e.what());
        }

        ch.Adapt(r_p, m_p, h2, r2, m2, MSP, ID12, U1, pkPCHBA, skPCHBA, sksPCHBA_1);
        verify_result = ch.Verify(h2, r_p, m_p, pkPCHBA);
        ASSERT_TRUE(verify_result);
    }
    average();
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}