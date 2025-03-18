#include "scheme/PBCH/PCH_DSS_2019.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCH_DSS_2019_Test : public BaseTest<TestParams>{
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

    int ks[] = {256, 512, 1024};

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
    PCH_DSS_2019_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCH_DSS_2019_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        PCH_DSS_2019 ch(GetParam().curve, GetParam().swap);

        std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
        const int SIZE_OF_ATTR = attr_list.size();
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
        Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, ch.GetZrElement());
        pg.generatePolicyInMatrixForm(binary_tree_expression);
        Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
        if(visiable){
            printf("Policy Matrix:\n");
            MSP->printMatrix();
        }

        PCH_DSS_2019_pp ppPCH;
        PCH_DSS_2019_sk skPCH;
        PCH_DSS_2019_pk pkPCH;
        PCH_DSS_2019_sks sksPCH;

        PCH_DSS_2019_h h;
        PCH_DSS_2019_r r,r_p;

        std::string m = "message to hash";
        std::string m_p = "message to adapt";

        this->start("SetUp");
        ch.SetUp(ppPCH, pkPCH, skPCH, GetParam().k);
        this->end("SetUp");

        this->start("KeyGen");
        ch.KeyGen(sksPCH, skPCH, pkPCH, attr_list);
        this->end("KeyGen");

        this->start("Hash");
        ch.Hash(h, r, m, MSP, POLICY, pkPCH, ppPCH);
        this->end("Hash");
        if(visiable){
            h.get_h().print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, m, pkPCH);
        this->end("Check");
        ASSERT_TRUE(check_result);
        
        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, r, m, sksPCH, pkPCH, MSP, POLICY);
        this->end("Adapt");
        if(visiable){
            r_p.get_rCHET().print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, r_p, m_p, pkPCH);
        this->end("Verify");
        ASSERT_TRUE(verify_result);
    }
    average();
}

int main(int argc, char **argv) 
{
    ::testing::GTEST_FLAG(catch_exceptions) = false;
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}