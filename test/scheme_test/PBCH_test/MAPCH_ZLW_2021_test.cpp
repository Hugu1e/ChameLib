#include "scheme/PBCH/MAPCH_ZLW_2021.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class MAPCH_ZLW_2021_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
            if(visiable)printf("%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};
    // int lamudas[] = {256, 512, 1024};
    int lamudas[] = {32, 64, 128};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            for (int lamuda : lamudas) {
                test_params.push_back({curve, authNum, lamuda});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    MAPCH_ZLW_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MAPCH_ZLW_2021_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        MAPCH_ZLW_2021 ch(GetParam().curve);

        const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
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

        // attribute
        const std::string A = "ONE";
        const std::string B = "TWO";
        const std::string C = "THREE";
        const std::string D = "FOUR";
        std::vector<std::string> As = {A, B, C, D};
        const int SIZE_OF_ATTRIBUTES = As.size();
        const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

        MAPCH_ZLW_2021_pp pp;

        CH_ET_BC_CDK_2017_pk pkCH;
        CH_ET_BC_CDK_2017_sk skCH;
        MA_ABE_gpk gpkABE;

        std::vector<MAPCH_ZLW_2021_mhk *> mhks;
        std::vector<MAPCH_ZLW_2021_mtk *> mtks;
        std::vector<MAPCH_ZLW_2021_mski *> mskis;
        MAPCH_ZLW_2021_h h;
        MAPCH_ZLW_2021_r r,r_p;

        std::string m = "message";
        std::string m_p = "message_p";
        
        this->start("GlobalSetup");
        ch.GlobalSetup(pkCH, skCH, gpkABE, pp, h, r, r_p, GetParam().lamuda);
        this->end("GlobalSetup");
        
        for(int i=0; i<GetParam().authNum; i++){
            MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
            MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();

            if (i < SIZE_OF_ATTRIBUTES) {
                this->start("AuthSetUp");
                ch.AuthSetUp(*mhk, *mtk, As[i], pkCH, skCH, gpkABE, pp);
                this->end("AuthSetUp");
            }else{
                this->start("AuthSetUp");
                ch.AuthSetUp(*mhk, *mtk, "ATTRIBUTE@AUTHORITY_" + std::to_string(i), pkCH, skCH, gpkABE, pp);
                this->end("AuthSetUp");
            }
            
            mhks.push_back(mhk);
            mtks.push_back(mtk);
        }

        
        for(int i=0; i<GetParam().authNum; i++){
            MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();
            if(i < SIZE_OF_ATTRIBUTES){
                this->start("KeyGen");
                ch.KeyGen(*mski, *mtks[i], *mhks[i], As[i], GID);
                this->end("KeyGen");
            }else{
                this->start("KeyGen");
                ch.KeyGen(*mski, *mtks[i], *mhks[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i), GID);
                this->end("KeyGen");
            }
            mskis.push_back(mski);
        }

        this->start("Hash");
        ch.Hash(h, r, m, pp, mhks, MSP, POLICY);
        this->end("Hash");
        if(visiable){
            h.get_h().print();
            r.get_r().print();
        }

        this->start("Check");
        bool check_result = ch.Check(h, r, m, mhks);
        this->end("Check");
        ASSERT_TRUE(check_result);

        // mskis.pop_back();
        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, r, m, mhks, mskis, MSP);
        this->end("Adapt");
        if(visiable){
            r_p.get_r().print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(h, r_p, m_p, mhks);
        this->end("Verify");
        ASSERT_TRUE(verify_result);
    }
    average();
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}