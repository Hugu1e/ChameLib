#include "scheme/CH/CH_KEF_NoMH_AM_2004.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int k;
};

const TestParams test_values[] = {
    {512}
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
            fprintf(out, "%s k %d\n", testName.c_str(), k);
            printf("%s k %d\n", testName.c_str(), k);
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
    CH_KEF_NoMH_AM_2004 ch = CH_KEF_NoMH_AM_2004();

    CH_KEF_NoMH_AM_2004_pk pk;
    CH_KEF_NoMH_AM_2004_sk sk;
    CH_KEF_NoMH_AM_2004_h h;
    CH_KEF_NoMH_AM_2004_r r, r_p;

    mpz_t m,m_p;
    mpz_inits(m, m_p, NULL);
    RandomGenerator::RandomInLength(m, 1024);
    RandomGenerator::RandomInLength(m_p, 1024);

    this->start("Setup");
    ch.Setup(pk, sk, h, r, r_p);
    this->end("Setup");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, GetParam().k);
    this->end("KeyGen");
    pk.print();
    sk.print();

    Logger::PrintGmp("m", m);
    this->start("Hash");
    ch.Hash(h, r, m, pk);
    this->end("Hash");
    h.print();
    r.print();


    this->start("Check");
    bool check_result = ch.Check(h, r, m, pk);
    this->end("Check");
    ASSERT_TRUE(check_result);

    Logger::PrintGmp("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pk, sk);
    this->end("Adapt");
    r_p.print();

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, pk);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

INSTANTIATE_TEST_CASE_P(
	CH_KEF_NoMH_AM_2004,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


