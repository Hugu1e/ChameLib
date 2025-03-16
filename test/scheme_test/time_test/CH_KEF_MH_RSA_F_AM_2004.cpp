#include "scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h"
#include "CommonTest.h"

struct TestParams{
	int k, tau, repeat;
};

const TestParams test_values[] = {
    {1024, 512, 1024}
};

class CH_KEF_MH_RSA_F_AM_2004_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            int k = GetParam().k;
            int tau = GetParam().tau;
            fprintf(out, "%s k %d tau %d\n", testName.c_str(), k, tau);
            if(visiable)printf("%s k %d tau %d\n", testName.c_str(), k, tau);
        }
};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSA_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSA_F_AM_2004_Test, Test){
    CH_KEF_MH_RSA_F_AM_2004 ch;
    int k = GetParam().k;
    int tau = GetParam().tau;
    int repeat = GetParam().repeat;

    CH_KEF_MH_RSA_F_AM_2004_pk pk[repeat];
    CH_KEF_MH_RSA_F_AM_2004_sk sk[repeat];
    mpz_t h[repeat];
    mpz_t B[repeat];
    mpz_t m[repeat], m_p[repeat];
    mpz_t label[repeat];
    mpz_t r[repeat], r_p[repeat];
    for(int i = 0; i < repeat; i++) {
        mpz_inits(h[i], B[i], r[i], m[i], label[i], r_p[i], m_p[i], NULL);
        RandomGenerator::RandomInLength(m[i], 1024);
        RandomGenerator::RandomInLength(m_p[i], 1024);
        RandomGenerator::RandomInLength(label[i], 1024);
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.SetUp(pk[i], sk[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Setup: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.KeyGen(pk[i], sk[i], k, tau);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("KeyGen: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) ch.Hash(h[i], r[i], B[i], pk[i], sk[i], m[i], label[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Hash: %lf ms\n", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Check(pk[i], m[i], label[i], h[i], r[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Check: %lf ms\n", duration * 1.0 / repeat);
        ASSERT_TRUE(total);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) ch.Adapt(r_p[i], pk[i], m[i], m_p[i], label[i], h[i], B[i], r[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        printf("Adapt: %lf ms\n", duration * 1.0 / repeat);
        ASSERT_TRUE(total);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        bool total = true;
        for(int i = 0;i < repeat;++i) total &= ch.Verify(pk[i], m_p[i], label[i], h[i], r_p[i]);
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

