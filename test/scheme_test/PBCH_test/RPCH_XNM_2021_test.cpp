#include "scheme/PBCH/RPCH_XNM_2021.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
    int k;
};

class PBCH_Test : public testing::TestWithParam<TestParams>{
    private:
        bool out_file = true;
        bool visiable = true;

        std::stack<std::string> current_test_name;
        std::stack<std::chrono::_V2::system_clock::time_point> ts;

        FILE *out = NULL;
    
    protected:
        void SetUp() override {
            std::string filename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
            size_t pos = filename.find('/');
            if (pos != std::string::npos) {
                filename = filename.substr(0, pos);
                filename += ".txt";
            }
            
            out = fopen(filename.c_str(), "a");
            fflush(out);

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d leafNodeSize: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize, GetParam().k);
            printf("%s %s swap: %d leafNodeSize: %d k: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize, GetParam().k);
        }

        void TearDown() override {
            fprintf(out, "\n\n");
            fclose(out);
        }

        void OutTime(std::string name, std::string id, double us) {
            us /= 1000;
            if(out_file) fprintf(out, "%s %s time: %lf ms.\n", name.c_str(), id.c_str(), us);
            if(visiable) printf("%s %s time: %lf ms.\n", name.c_str(), id.c_str(), us);
        }
        
        void start(std::string current_test_name) {
            std::cout<<"——————————" << current_test_name <<" start——————————" << std::endl;
            this->current_test_name.push(current_test_name);
            ts.push(std::chrono::system_clock::now());
        }

        void end(std::string current_test_name) {
            std::chrono::_V2::system_clock::time_point te = std::chrono::system_clock::now();
            if(this->current_test_name.empty() || this->current_test_name.top() != current_test_name) throw std::runtime_error("end(): wrong test pair");
            OutTime(current_test_name, ::testing::UnitTest::GetInstance()->current_test_info()->name(), std::chrono::duration_cast<std::chrono::microseconds>(te - ts.top()).count());
            std::cout<<"——————————" << current_test_name <<" end——————————" << std::endl;
            this->current_test_name.pop();
            ts.pop();
        }
};

TEST_P(PBCH_Test, Test){
    RPCH_XNM_2021 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();  // S
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;
    const time_t T = TimeUtils::TimeCast(2024, 12, 21, 0, 0, 0);  // present time

    const time_t end_time_1 = TimeUtils::TimeCast(2025, 12, 31, 0, 0, 0);

    RPCH_XNM_2021_sk skRPCH;
    RPCH_XNM_2021_pk pkRPCH;
    RPCH_XNM_2021_skid skidRPCH;
    RPCH_XNM_2021_dkidt dkidtRPCH;
    RPCH_XNM_2022_kut kut;

    RPCH_XNM_2021_h h,h_p;

    RPCH_XNM_2021_RevokedPresonList rl;
    RPCH_XNM_2022_Binary_tree st;

    element_s *id = ch.GetZrElement();
    mpz_t m,m_p;
    mpz_inits(m,m_p,NULL);
    RandomGenerator::RandomInLength(m, 128);
    RandomGenerator::RandomInLength(m_p, 128);
 
    this->start("SetUp");
    ch.SetUp(skRPCH, pkRPCH, rl, st, GetParam().k, GetParam().leafNodeSize);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(skidRPCH, pkRPCH, skRPCH, st, attr_list, id, end_time_1);
    this->end("KeyGen");

    this->start("KUpt");
    ch.KUpt(kut, pkRPCH, st, rl, T);
    this->end("KUpt");
    printf("size of kut.ku_theta: %ld\n", kut.get_kut().get_ku_theta().size());

    this->start("DKGen");
    ch.DKGen(dkidtRPCH, pkRPCH, skidRPCH, kut);
    this->end("DKGen");

    this->start("Rev");
    ch.Rev(rl, id, T);
    this->end("Rev");

    Logger::PrintGmp("m", m);
    this->start("Hash");
    ch.Hash(h, m, pkRPCH, POLICY, T);
    this->end("Hash");
    // h1 h2 r1 r2 N2 cSE
    h.get_h().print();
    h.get_r().print();

    this->start("Check");
    bool check = ch.Check(pkRPCH, m, h);
    this->end("Check");
    ASSERT_TRUE(check);

    Logger::PrintGmp("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(h_p, m_p, m, h, pkRPCH, dkidtRPCH);
    this->end("Adapt");
    // h1 h2 r1 r2 N2 cSE
    h_p.get_h().print();
    h_p.get_r().print();

    this->start("Verify");
    bool verify = ch.Verify(pkRPCH, m_p, h_p);
    this->end("Verify");
    ASSERT_TRUE(verify);
}

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

    int ks[] = {128, 256, 512};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                for (int k : ks) {
                    test_params.push_back({curve, swap, leafNodeSize, k});
                }
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	RPCH_XNM_2021,
	PBCH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}