#include "scheme/PBCH/RPCH_XNM_2021.h"
#include "CommonTest.h"
#include <chrono>

struct TestParams{
	int curve;
    int repeat;
};

class Base_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();
            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
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
    int repeat[] = {1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int rep : repeat) {
            test_params.push_back({curve, rep});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	scheme_test,
    Base_Test,
	testing::ValuesIn(test_values)
);

TEST_P(Base_Test, Test){
    auto curve = GetParam().curve;
    printf("curve: %d\n", curve);
    auto repeat = GetParam().repeat;
    
    PbcScheme ch(GetParam().curve);
    ch.defaultInit();

    element_s *G1s[repeat];
    for(int i = 0;i < repeat;++i) G1s[i] = ch.GetG1Element();
    element_s *G2s[repeat];
    for(int i = 0;i < repeat;++i) G2s[i] = ch.GetG2Element();
    element_s *GTs[repeat];
    for(int i = 0;i < repeat;++i) GTs[i] = ch.GetGTElement();
    element_s *Zps[repeat];
    for(int i = 0;i < repeat;++i) Zps[i] = ch.GetZrElement();
    {
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) element_random(G1s[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("random in G1: %lf ms\n", duration * 1.0 / repeat);
            printf("{%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) element_random(G2s[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("random in G2: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) element_random(GTs[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("random in GT: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) element_random(Zps[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("random in Zr: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
    }
    
    {
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) HASH::hash(G1s[i], G1s[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("hash to G1: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) HASH::hash(G2s[i], G2s[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("hash to G2: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) HASH::hash(GTs[i], GTs[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("hash to GT: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int i = 0;i < repeat;++i) HASH::hash(Zps[i], Zps[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // printf("hash to Zr: %lf ms\n", duration * 1.0 / repeat);
            printf("%lf, ", duration * 1.0 / repeat);
        }
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_mul(G1s[i], G1s[i], G1s[(i + 1) % repeat]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("add in G1: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_mul(G2s[i], G2s[i], G2s[(i + 1) % repeat]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("add in G2: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_mul(GTs[i], GTs[i], GTs[(i + 1) % repeat]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("add in GT: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_mul(Zps[i], Zps[i], Zps[(i + 1) % repeat]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("add in Zr: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_pow_zn(G1s[i], G1s[i], Zps[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("mul in G1: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_pow_zn(G2s[i], G2s[i], Zps[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("mul in G2: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_pow_zn(GTs[i], GTs[i], Zps[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("mul in GT: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_pow_zn(Zps[i], Zps[i], Zps[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("mul in Zr: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf, ", duration * 1.0 / repeat);
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0;i < repeat;++i) element_pairing(GTs[i], G1s[i], G2s[i]);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // printf("pairing: %lf ms\n", duration * 1.0 / repeat);
        printf("%lf},\n", duration * 1.0 / repeat);
    }

    // element_s *G1 = ch.GetG1Element();
    // element_s *G2 = ch.GetG2Element();
    // element_s *GT = ch.GetGTElement();
    // element_s *Zp = ch.GetZrElement();

}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}