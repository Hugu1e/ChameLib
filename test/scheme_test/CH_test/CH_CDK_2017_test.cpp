#include "scheme/CH/CH_CDK_2017.h"
#include "CommonTest.h"
#include <gtest/gtest.h>

struct TestParams{
	int lamuda;
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
        }

        void TearDown() override {
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
    CH_CDK_2017 ch;

    CH_CDK_2017_pk pk;
    CH_CDK_2017_sk sk;
    CH_CDK_2017_h h;
    CH_CDK_2017_r r, r_p;
    mpz_t m, tag, m_p, tag_p;
    mpz_inits(m, tag, m_p, tag_p, NULL);

    this->start("SetUp");
    ch.SetUp(pk, sk, h, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, 1024);
    this->end("KeyGen");

    RandomGenerator::RandomN(m, pk.get_rsa_pk()[AE_RSA::n]);
    RandomGenerator::RandomN(tag, pk.get_rsa_pk()[AE_RSA::n]);
    Logger::PrintGmp("m", m);
    Logger::PrintGmp("tag", tag);
    this->start("Hash");
    ch.Hash(h, r, m, tag, pk);
    this->end("Hash");
    h.print();
    r.print();

    this->start("Check");
    bool check_result = ch.Check(h, r, m, tag, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    RandomGenerator::RandomN(m_p, pk.get_rsa_pk()[AE_RSA::n]);
    Logger::PrintGmp("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, tag_p, h, r, m_p, sk, pk);
    this->end("Adapt");
    r_p.print();
    Logger::PrintGmp("tag_p", tag_p);

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, tag_p, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

const TestParams test_values[] = {
    {256},
    {512},
    {1024},
    {2048}
};

INSTANTIATE_TEST_CASE_P(
	CH_CDK_2017,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

