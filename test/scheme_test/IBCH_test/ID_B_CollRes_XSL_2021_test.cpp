#include "scheme/IBCH/ID_B_CollRes_XSL_2021.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    bool swap;
    int length;
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
            
            fprintf(out, "%s %s swap:%d length:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().length);
            printf("%s %s swap:%d length:%d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().length);
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
    ID_B_CollRes_XSL_2021 ch(GetParam().curve, GetParam().swap);

    ID_B_CollRes_XSL_2021_pp pp;
    ID_B_CollRes_XSL_2021_msk msk;
    ID_B_CollRes_XSL_2021_tk tk;
    ID_B_CollRes_XSL_2021_h h;
    ID_B_CollRes_XSL_2021_r r,r_p;

    std::string I = "IDENTITY";
    
    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();    

    this->start("SetUp");
    ch.SetUp(pp, msk, tk, h, r, r_p, GetParam().length);
    this->end("SetUp");
    // pp.print();
    msk.print();

    this->start("KeyGen");
    ch.KeyGen(tk, msk, I.c_str(), pp);
    this->end("KeyGen");
    tk.print();

    Logger::PrintPbc("m", m);
    this->start("Hash");
    ch.Hash(h, r, m, I.c_str(), pp);
    this->end("Hash");
    h.print();
    r.print();

    this->start("Check");
    bool check_result = ch.Check(h, m, r, I.c_str(), pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    Logger::PrintPbc("m_p", m_p);
    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, tk);
    this->end("Adapt");
    h.print();
    r.print();
    
    this->start("Verify");
    bool verify_result = ch.Verify(h, m_p, r_p, I.c_str(), pp);
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

    bool swaps[] = {false, true};

    int lengths[] = {64, 128, 256};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for(int len: lengths){
                test_params.push_back({curve, swap, len});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	ID_B_CollRes_XSL_2021,
	IBCH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



