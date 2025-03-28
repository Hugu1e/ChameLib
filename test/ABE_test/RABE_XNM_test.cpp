#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
};

class RABE_XNM_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
            if(visiable)printf("%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
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

    int leadNodeSizes[] = {8, 16, 32, 64};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                test_params.push_back({curve, swap, leafNodeSize});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	ABE_Test,
    RABE_XNM_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RABE_XNM_Test, Test){
    RABE_XNM abe(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE", "TWO", "THREE"};
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // Compute MSP
    Policy_resolution pr;
    Policy_generation pg;
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    element_s *id1[repeat], *id2[repeat], *id3[repeat];
    for (int i = 0; i < repeat; i++) { id1[i] = abe.GetZrElement(); id2[i] = abe.GetZrElement(); id3[i] = abe.GetZrElement(); }

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // Present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);

    RABE_XNM_mpk mpk[repeat];
    RABE_XNM_msk msk[repeat];
    RABE_XNM_skid skid1[repeat], skid2[repeat], skid3[repeat];
    RABE_XNM_kut kut[repeat];
    RABE_XNM_dkidt dkidt1[repeat], dkidt2[repeat], dkidt3[repeat];
    RABE_XNM_ciphertext ciphertext[repeat];

    std::vector<RABE_XNM_revokedPreson> rl[repeat];
    Binary_tree_RABE st[repeat];

    element_s *msg[repeat], *res[repeat], *s1[repeat], *s2[repeat];
    for (int i = 0; i < repeat; i++) { msg[i] = abe.GetGTElement(); res[i] = abe.GetGTElement(); s1[i] = abe.GetZrElement(); s2[i] = abe.GetZrElement(); }

    this->start("Setup");
    for (int i = 0; i < repeat; i++) abe.Setup(mpk[i], msk[i], rl[i], st[i], GetParam().leafNodeSize);
    this->end("Setup");

    this->start("KeyGen");
    for (int i = 0; i < repeat; i++) abe.KGen(skid1[i], st[i], mpk[i], msk[i], attr_list, id1[i], re_time_1);
    this->end("KeyGen");
    for (int i = 0; i < repeat; i++) abe.KGen(skid2[i], st[i], mpk[i], msk[i], attr_list, id2[i], re_time_2);
    for (int i = 0; i < repeat; i++) abe.KGen(skid3[i], st[i], mpk[i], msk[i], attr_list, id3[i], re_time_3);

    this->start("Rev");
    for (int i = 0; i < repeat; i++) abe.Rev(rl[i], id1[i], re_time_1);
    this->end("Rev");
    for (int i = 0; i < repeat; i++) abe.Rev(rl[i], id2[i], re_time_2);
    for (int i = 0; i < repeat; i++) abe.Rev(rl[i], id3[i], re_time_3);

    this->start("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Enc(ciphertext[i], mpk[i], msg[i], MSP, T_present, s1[i], s2[i]);
    this->end("Encrypt");

    this->start("KUpt");
    for (int i = 0; i < repeat; i++) abe.KUpt(kut[i], mpk[i], st[i], rl[i], T_present);
    this->end("KUpt");

    for (int i = 0; i < repeat; i++) {
        try { abe.DKGen(dkidt1[i], mpk[i], skid1[i], kut[i]); } catch (const std::runtime_error& e) {}
        try { abe.DKGen(dkidt3[i], mpk[i], skid3[i], kut[i]); } catch (const std::runtime_error& e) {}
    }

    this->start("DKGen");
    for (int i = 0; i < repeat; i++) abe.DKGen(dkidt2[i], mpk[i], skid2[i], kut[i]);
    this->end("DKGen");

    this->start("Decrypt");
    for (int i = 0; i < repeat; i++) abe.Dec(res[i], mpk[i], ciphertext[i], dkidt2[i], MSP);
    this->end("Decrypt");

    for (int i = 0; i < repeat; i++) ASSERT_TRUE(element_cmp(msg[i], res[i]) == 0);

    average();

    int n = MSP->row();
    int m = MSP->col();
    int HT = log2(GetParam().leafNodeSize);
    int UTS = kut[0].get_ku_theta().size();

    int op_cnt_Setup[] = {
        1, 1, 0, 7, 
        0, 0, 0, 0, 
        0, 0, 0, 2, 
        3, 2, 2, 0, 
        1
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Setup, "Setup"));

    int op_cnt_KeyGen[] = {
        0, 0, 0, 3 + n, 
        6 + 6 * n, 0, 0, 0, 
        9 + 6 * n + HT, 0, 0, 10 + 2 * n, 
        9 + 9 * n, 3, 0, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

    int op_cnt_KUpt[] = {
        0, 0, 0, UTS, 
        UTS, 0, 0, 0, 
        UTS, 0, 0, 0, 
        UTS, UTS, 0, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KUpt, "KUpt"));

    int op_cnt_DKGen[] = {
        0, 0, 0, 1, 
        1, 0, 0, 0, 
        2, 1, 0, 0, 
        1, 1, 0, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_DKGen, "DKGen"));

    int op_cnt_Encrypt[] = {
        0, 0, 0, 0, 
        3 * n * (2 * m + 2) + 1, 0, 0, 0, 
        3 * n * (2 * m + 1), 0, 2, 0, 
        3 * n * (3 * m + 2) + 1, 3, 2, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Encrypt, "Encrypt"));

    int op_cnt_Decrypt[] = {
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        6 * n, 0, 7, 0, 
        6 * n, 0, 0, 0, 
        7
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Decrypt, "Decrypt"));
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