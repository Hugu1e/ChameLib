#include "scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h"
#include "CommonTest.h"
#include "cal_diff.h"

struct TestParams{
	int curve, repeat;
};

class CH_KEF_MH_SDH_DL_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s\n", testName.c_str(), curveName.c_str());
            if(visiable)printf("%s %s\n", testName.c_str(), curveName.c_str());
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve, 100});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_SDH_DL_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_SDH_DL_AM_2004_Test, Test){
    int curve = GetParam().curve;
    int repeat = GetParam().repeat;
    CH_KEF_MH_SDH_DL_AM_2004 ch(curve);
        
    CH_KEF_MH_SDH_DL_AM_2004_pp pp[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_sk sk[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_pk pk[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_h h[repeat];
    CH_KEF_MH_SDH_DL_AM_2004_r r[repeat], r_p[repeat];

    element_s *m[repeat];
    element_s *m_p[repeat];
    element_s *label[repeat];
    for(int i = 0; i < repeat; i++) {
        m[i] = ch.GetZrElement();
        m_p[i] = ch.GetZrElement();
        label[i] = ch.GetZrElement();
    }

    int op_cnt[][diff_max_len] = {
        {
            1, 0, 0, 0, 
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            0
        }, //0, setup

        {
            0, 0, 0, 1, 
            0, 0, 0, 0, 
            0, 0, 0, 0, 
            1, 0, 0, 0, 
            0
        }, //1, keygen
        
        {
            0, 0, 0, 2, 
            0, 0, 0, 2, 
            2, 0, 0, 0, 
            4, 0, 0, 0, 
            0
        }, //2, hash

        {
            0, 0, 0, 0, 
            0, 0, 0, 2, 
            2, 0, 0, 0, 
            2, 0, 0, 0, 
            2
        }, //3, check

        {
            0, 0, 0, 0, 
            0, 0, 0, 3, 
            1, 0, 0, 2, 
            1, 0, 0, 1, 
            0
        }, //4, adapt
    };

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.SetUp(pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("SetUp: %lf ms\n", duration * 1.0 / repeat);
        printf("SetUp: ");
        ASSERT_TRUE(out_time(curve, op_cnt[0], duration * 1.0 / repeat));
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.KeyGen(pk[i], sk[i], pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("KeyGen: %lf ms\n", duration * 1.0 / repeat);
        printf("KeyGen: ");
        ASSERT_TRUE(out_time(curve, op_cnt[1], duration * 1.0 / repeat));
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.Hash(h[i], r[i], m[i], label[i], pk[i], pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("Hash: %lf ms\n", duration * 1.0 / repeat);
        printf("Hash: ");
        ASSERT_TRUE(out_time(curve, op_cnt[2], duration * 1.0 / repeat));
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Check(h[i], r[i], m[i], label[i], pk[i], pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("Check: %lf ms\n", duration * 1.0 / repeat);
        printf("Check: ");
        ASSERT_TRUE(out_time(curve, op_cnt[3], duration * 1.0 / repeat));
        ASSERT_TRUE(total);
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) ch.Adapt(r_p[i], m_p[i], h[i], r[i], m[i], label[i], sk[i], pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("Adapt: %lf ms\n", duration * 1.0 / repeat);
        printf("Adapt: ");
        ASSERT_TRUE(out_time(curve, op_cnt[4], duration * 1.0 / repeat));
        ASSERT_TRUE(total);
    }
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Verify(h[i], r_p[i], m_p[i], label[i], pk[i], pp[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("Verify: %lf ms\n", duration * 1.0 / repeat);
        printf("Verify: ");
        ASSERT_TRUE(out_time(curve, op_cnt[3], duration * 1.0 / repeat));
        ASSERT_TRUE(total);
    }
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






