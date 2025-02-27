#include "scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
};

class CH_Test : public testing::TestWithParam<TestParams>{
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
            fprintf(out, "%s %s\n", testName.c_str(), curveName.c_str());
            printf("%s %s\n", testName.c_str(), curveName.c_str());
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

TEST_P(CH_Test, Test){
    CH_KEF_MH_SDH_DL_AM_2004 ch(GetParam().curve);
        
    CH_KEF_MH_SDH_DL_AM_2004_pp pp;
    CH_KEF_MH_SDH_DL_AM_2004_sk sk;
    CH_KEF_MH_SDH_DL_AM_2004_pk pk;
    CH_KEF_MH_SDH_DL_AM_2004_h h;
    CH_KEF_MH_SDH_DL_AM_2004_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *label = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    Logger::PrintPbc("label", label);
    Logger::PrintPbc("m", m);
    this->start("Hash");
    ch.Hash(h, r, m, label, pk, pp);
    this->end("Hash");
    Logger::PrintPbc("Hash value", h[0]);
    Logger::PrintPbc("r", r[0]);

    this->start("Check");
    bool check_result = ch.Check(h, r, m, label, pk, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    Logger::PrintPbc("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, label, sk, pp);
    this->end("Adapt");
    Logger::PrintPbc("r_p", r_p[0]);

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, label, pk, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };


    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_KEF_MH_SDH_DL_AM_2004,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






