#include "scheme/CH/CHET_RSA_CDK_2017.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {128},
    {256},
    {512},
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
            int lamuda = GetParam().lamuda;
            fprintf(out, "%s lamuda %d\n", testName.c_str(), lamuda);
            printf("%s lamuda %d\n", testName.c_str(), lamuda);
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
    CHET_RSA_CDK_2017 ch;
    CHET_RSA_CDK_2017_pk pk;
    CHET_RSA_CDK_2017_sk sk;
    CHET_RSA_CDK_2017_h h;
    CHET_RSA_CDK_2017_etd etd;

    mpz_t r, r_p;
    mpz_t m, m_p;
    mpz_inits(r, r_p, m, m_p, NULL);

    this->start("SetUp");
    ch.SetUp(pk, sk, h, etd);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().lamuda);
    this->end("KeyGen");
    pk.print();
    sk.print();

    RandomGenerator::RandomInLength(m, 100);
    Logger::PrintGmp("m", m);
    this->start("Hash");
    ch.Hash(h, r, etd, m, pk);
    this->end("Hash");
    h.print();
    Logger::PrintGmp("r", r);
    etd.print();

    this->start("Check");
    bool check_result = ch.Check(h, r, m, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    RandomGenerator::RandomInLength(m_p, 100);
    Logger::PrintGmp("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, m_p, m, r, h, sk, etd, pk);
    this->end("Adapt");
    Logger::PrintGmp("r_p", r_p);

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

INSTANTIATE_TEST_CASE_P(
	CHET_RSA_CDK_2017,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

