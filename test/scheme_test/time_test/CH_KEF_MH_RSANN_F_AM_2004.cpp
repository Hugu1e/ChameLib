#include "scheme/CH/CH_KEF_MH_RSANN_F_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k, repeat;
};

const TestParams test_values[] = {
    {512, 1024}
};

class CH_KEF_MH_RSANN_F_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            int k = GetParam().k;
            fprintf(out, "%s k %d\n", testName.c_str(), k);
            if(visiable)printf("%s k %d\n", testName.c_str(), k);
        }
};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSANN_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSANN_F_AM_2004_Test, Test){
    CH_KEF_MH_RSANN_F_AM_2004 ch;
    int k = GetParam().k;
    int repeat = GetParam().repeat;

    CH_KEF_MH_RSANN_F_AM_2004_pk pk[repeat];
    CH_KEF_MH_RSANN_F_AM_2004_sk sk[repeat];
    CH_KEF_MH_RSANN_F_AM_2004_h h[repeat];
    CH_KEF_MH_RSANN_F_AM_2004_r r[repeat], r_p[repeat];
    
    mpz_t m[repeat], m_p[repeat];
    mpz_t label[repeat];
    
    for(int i = 0; i < repeat; i++) {
        mpz_inits(m[i], m_p[i], label[i], NULL);
        RandomGenerator::RandomPrimeInLength(m[i], k);
        RandomGenerator::RandomInLength(m_p[i], 64);
        RandomGenerator::RandomInLength(label[i], 64);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.SetUp(pk[i], sk[i], h[i], r[i], r_p[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Setup: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.KeyGen(pk[i], sk[i], k);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("KeyGen: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.Hash(h[i], r[i], m[i], label[i], pk[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Hash: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Check(h[i], m[i], r[i], label[i], pk[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Check: %lf ms\n", duration * 1.0 / repeat);
        ASSERT_TRUE(total);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) ch.Adapt(r_p[i], m_p[i], h[i], m[i], r[i], label[i], sk[i], pk[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Adapt: %lf ms\n", duration * 1.0 / repeat);
        ASSERT_TRUE(total);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Verify(h[i], m_p[i], r_p[i], label[i], pk[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Verify: %lf ms\n", duration * 1.0 / repeat);
        ASSERT_TRUE(total);
    }
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


