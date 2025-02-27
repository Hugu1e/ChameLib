#include "scheme/CH/CR_CH_DSS_2020.h"
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
    if(GetParam().group == Group::G2) {
        switch (GetParam().curve) {
            case Curve::A:
            case Curve::A1:
            case Curve::E:
                break;

            default:
                return;
        }
    }

    CR_CH_DSS_2020 ch(GetParam().curve, GetParam().group);

    CR_CH_DSS_2020_pp pp;
    CR_CH_DSS_2020_pk pk;
    CR_CH_DSS_2020_sk sk;
    CR_CH_DSS_2020_h h;
    CR_CH_DSS_2020_r r,r_p;

    element_s *m = ch.GetG1Element();
    element_s *m_p = ch.GetG1Element();

    this->start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p);
    this->end("SetUp");
    pp.print();

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");
    pk.print();
    sk.print();

    Logger::PrintPbc("m", m);
    this->start("Hash");
    ch.Hash(h, r, m, pk, pp);
    this->end("Hash");
    h.print();
    r.print();

    this->start("Check");
    bool check_result = ch.Check(h, r, m, pk, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    Logger::PrintPbc("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, h, r, m, m_p, sk, pk, pp);
    this->end("Adapt");
    r_p.print();

    this->start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, pk, pp);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
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
	CR_CH_DSS_2020,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






