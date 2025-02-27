#include "scheme/CH/CH_KEF_DLP_LLA_2012.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    int group;
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
            std::string groupName;
            if (GetParam().group == Group::G1) {
                groupName = "G1";
            } else if (GetParam().group == Group::G2) {
                groupName = "G2";
            } else if (GetParam().group == Group::GT) {
                groupName = "GT";
            } else {
                groupName = "UNKNOWN";
            }
            fprintf(out, "%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
            printf("%s %s %s\n", testName.c_str(), curveName.c_str(), groupName.c_str());
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
    CH_KEF_DLP_LLA_2012 ch(GetParam().curve, GetParam().group);

    CH_KEF_DLP_LLA_2012_pp pp;
    CH_KEF_DLP_LLA_2012_sk sk;
    CH_KEF_DLP_LLA_2012_pk pk;
    CH_KEF_DLP_LLA_2012_label label;
        
    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *m_pp = ch.GetZrElement();
    element_s *r = ch.GetZrElement();
    element_s *r_p = ch.GetZrElement();
    element_s *r_pp = ch.GetZrElement();
    element_s *h = ch.GetG1Element();

    Logger::PrintPbc("m", m);
    Logger::PrintPbc("m_p", m_p);
    Logger::PrintPbc("m_pp", m_pp);


    this->start("SetUp");
    ch.SetUp(pp, pk, sk, label);
    this->end("SetUp");
    // pp.printElement("g");

    this->start("KeyGen");
    ch.KeyGen(sk, pk, label, pp);
    this->end("KeyGen");
    // sk.printElement("a");
    // sk.printElement("x1");
    // sk.printElement("x2");

    this->start("Hash");
    ch.Hash(h, r, pk, m, label, pp);
    this->end("Hash");
    Logger::PrintPbc("Hash value", h);
    Logger::PrintPbc("r", r);

    this->start("Check");
    bool check_result = ch.Check(m, r, pk, label, h, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("UForge");
    ch.UForge(r_p, sk, pk, label, h, m, m_p, r, pp);
    this->end("UForge");
    Logger::PrintPbc("r_p", r_p);

    this->start("Verify");
    bool verify_result_1 = ch.Verify(m_p, r_p, pk, label, h, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result_1);

    this->start("IForge");
    ch.IForge(r_pp, label, m, m_p, r, r_p, m_pp);
    this->end("IForge");
    Logger::PrintPbc("r_pp", r_pp);

    this->start("Verify");
    bool verify_result_2 = ch.Verify(m_pp, r_pp, pk, label, h, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result_2);

    
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

    int groups[] = {Group::G1, Group::G2, Group::GT};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int group : groups) {
            test_params.push_back({curve, group});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_KEF_DLP_LLA_2012,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






