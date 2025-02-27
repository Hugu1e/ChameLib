#include "scheme/CH/CH_KEF_MH_RSA_F_AM_2004.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int k;
    int tau;
};

const TestParams test_values[] = {
    {1024, 512}
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
            int k = GetParam().k;
            int tau = GetParam().tau;
            fprintf(out, "%s k %d tau %d\n", testName.c_str(), k, tau);
            printf("%s k %d tau %d\n", testName.c_str(), k, tau);
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
    CH_KEF_MH_RSA_F_AM_2004 ch;

    CH_KEF_MH_RSA_F_AM_2004_pk pk;
    CH_KEF_MH_RSA_F_AM_2004_sk sk;
    mpz_t h;
    mpz_t B;
    mpz_t m,m_p;
    mpz_t label;
    mpz_t r, r_p;
    mpz_inits(h, B, r, m, label, r_p, m_p, NULL);

    this->start("SetUp");
    ch.SetUp(pk, sk);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().k, GetParam().tau);
    this->end("KeyGen");

    
    mpz_set_ui(m, 42525346346746);
    mpz_set_ui(label, 424253532414);
    this->start("Hash");
    ch.Hash(h, r, B, pk, sk, m, label);
    this->end("Hash");
    Logger::PrintGmp("h", h);
    Logger::PrintGmp("r", r);
    Logger::PrintGmp("B", B);
    

    this->start("Check");
    bool check_result = ch.Check(pk, m, label, h, r);
    this->end("Check");
    ASSERT_TRUE(check_result);

    mpz_set_ui(m_p, 96725346346246);
    this->start("Adapt");
    ch.Adapt(r_p, pk, m, m_p, label, h, B, r);
    this->end("Adapt");
    Logger::PrintGmp("r_p", r_p);

    this->start("Verify");
    bool verify_result = ch.Verify(pk, m_p, label, h, r_p);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

INSTANTIATE_TEST_CASE_P(
	CH_KEF_MH_RSA_F_AM_2004,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

