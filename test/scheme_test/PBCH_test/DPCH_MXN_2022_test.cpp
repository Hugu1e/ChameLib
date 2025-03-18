#include "scheme/PBCH/DPCH_MXN_2022.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class DPCH_MXN_2022_Test : public BaseTest<TestParams>{
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
    int lamudas[] = {256, 512, 1024};

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
    DPCH_MXN_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(DPCH_MXN_2022_Test, Test){    
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        DPCH_MXN_2022 ch(GetParam().curve);

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
        const std::string ATTRIBUTES[] = {A, B, C, D};
        const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);
        const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

        std::vector<DPCH_MXN_2022_pkTheta *> pkThetas;
        std::vector<DPCH_MXN_2022_skTheta *> skThetas;
        std::vector<DPCH_MXN_2022_skGidA *> skGidAs;

        DPCH_MXN_2022_pp ppDPCH;
        DPCH_MXN_2022_pk pkDPCH;
        DPCH_MXN_2022_sk skDPCH;
        DPCH_MXN_2022_skGid skGid;
        DPCH_MXN_2022_sigmaGid sigmaGid;
        DPCH_MXN_2022_h h;
        DPCH_MXN_2022_r r,r_p;
        std::string m = "message";
        std::string m_p = "message_p";

        this->start("SetUp");
        ch.SetUp(ppDPCH, pkDPCH, skDPCH, h, r, r_p, sigmaGid, skGid, GetParam().lamuda);
        this->end("SetUp");
        if(visiable){
            ppDPCH.get_gpk_MA_ABE().print();
            pkDPCH.get_pk_CH().print();
            skDPCH.get_sk_CH().print();
            pkDPCH.get_pk_BLS().print();
            skDPCH.get_sk_BLS().print();
        }

        this->start("ModSetUp");
        ch.ModSetUp(skGid, sigmaGid, skDPCH, GID);
        this->end("ModSetUp");
        if(visiable){
            skGid.get_sk_CH().print();
            sigmaGid.get_signature().print();
        }

        
        for(int i = 0; i < GetParam().authNum; i++) {
            DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
            DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;

            if(i < SIZE_OF_ATTRIBUTES){
                this->start("AuthSetUp");
                ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[i]);
                this->end("AuthSetUp");
            }else{
                this->start("AuthSetUp");
                ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
                this->end("AuthSetUp");
            }

            pkThetas.push_back(pkTheta);
            skThetas.push_back(skTheta);
            
            // if(visiable){
            //     pkTheta->get_pk().print();
            //     skTheta->get_sk().print();
            // }
        }

        for(int i = 0; i < GetParam().authNum; i++) {
            DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;

            if (i < SIZE_OF_ATTRIBUTES) {
                this->start("ModKeyGen");
                ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], ATTRIBUTES[i]);
                this->end("ModKeyGen");
            } else {
                this->start("ModKeyGen");
                ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
                this->end("ModKeyGen");
            }
            
            skGidAs.push_back(skGidA);
            
            // if(visiable){
            //     printf("skGidA->gid: %s\n", skGidA->get_sk().get_gid().c_str());
            //     printf("skGidA->A: %s\n", skGidA->get_sk().get_A().c_str());
            //     skGidA->get_sk().print();
            // }
        }

        this->start("Hash");
        ch.Hash(h, r, m, ppDPCH, pkDPCH, pkThetas, MSP, POLICY);
        this->end("Hash");
        if(visiable){
            // h0, h1, r0, r1
            h.get_h().print();
            r.get_r().print();
        }

        this->start("Check");
        bool check_result = ch.Check(pkDPCH, m, h, r);
        this->end("Check");
        ASSERT_TRUE(check_result);

        // choose partial secret keys
        std::vector<DPCH_MXN_2022_skGidA *> _skGidAs;
        _skGidAs.push_back(skGidAs[0]);
        _skGidAs.push_back(skGidAs[1]);
        _skGidAs.push_back(skGidAs[2]);

        this->start("Adapt");
        ch.Adapt(r_p, m_p, h, r, m, pkDPCH, skGid, _skGidAs, ppDPCH, pkThetas, MSP, POLICY);
        this->end("Adapt");
        if(visiable){
            // r0, r1
            r_p.get_r().print();
        }

        this->start("Verify");
        bool verify_result = ch.Verify(pkDPCH, m_p, h, r_p);
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