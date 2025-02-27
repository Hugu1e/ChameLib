#include "scheme/IBCH/IB_CH_MD_LSX_2022.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
};

class IBCH_Test : public testing::TestWithParam<TestParams>{
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

TEST_P(IBCH_Test, Test){
    IB_CH_MD_LSX_2022 ch(GetParam().curve);

    IB_CH_MD_LSX_2022_pp pp;
    IB_CH_MD_LSX_2022_msk msk;
    IB_CH_MD_LSX_2022_td td;
    IB_CH_MD_LSX_2022_h h;
    IB_CH_MD_LSX_2022_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *ID = ch.GetZrElement();

    this->start("SetUp");
    ch.SetUp(pp, msk, td, h, r, r_p);
    this->end("SetUp");
    pp.print();
    msk.print();

    Logger::PrintPbc("ID", ID);
    this->start("KeyGen");
    ch.KeyGen(td, ID, msk, pp);
    this->end("KeyGen");
    td.print();


    Logger::PrintPbc("m", m);
    this->start("Hash");
    ch.Hash(h, r, ID, m, pp);
    this->end("Hash");
    h.print();
    r.print();


    this->start("Check");
    bool check_result = ch.Check(h, r, ID, m, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    Logger::PrintPbc("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, h, m, r, m_p, td);
    this->end("Adapt");
    r_p.print();


    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, ID, m_p, pp);
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
	IB_CH_MD_LSX_2022,
	IBCH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






