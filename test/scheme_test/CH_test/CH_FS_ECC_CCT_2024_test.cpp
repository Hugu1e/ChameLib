#include "scheme/CH/CH_FS_ECC_CCT_2024.h"
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
    if(GetParam().group == Group::G2){
        switch (GetParam().curve)
        {
        case Curve::A:
        case Curve::A1:
        case Curve::E:
            break;
        default:
            return;
        }
    }
    if(GetParam().group == Group::GT && GetParam().curve == Curve::G_149){
        return;
    }

    CH_FS_ECC_CCT_2024 ch(GetParam().curve, GetParam().group);

    CH_FS_ECC_CCT_2024_pp pp;
    CH_FS_ECC_CCT_2024_pk pk;
    CH_FS_ECC_CCT_2024_sk sk;
    CH_FS_ECC_CCT_2024_r r,r_p;
    
    element_s* m = ch.GetZrElement();
    element_s* m_p = ch.GetZrElement();

    element_s* h = ch.GetG1Element();

    this->start("SetUp");
    ch.SetUp(pp, pk, sk, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, pk, m, pp);
    this->end("Hash");
    r.print(CH_FS_ECC_CCT_2024::c1);
    r.print(CH_FS_ECC_CCT_2024::z1);
    r.print(CH_FS_ECC_CCT_2024::z2);
    Logger::PrintPbc("h", h);

    this->start("Check");
    bool check_result = ch.Check(pk, m, h, r, pp);
    this->end("Check");
    ASSERT_TRUE(check_result);

    this->start("Adapt");
    ch.Adapt(r_p, pk, sk, m, m_p, h, r, pp);
    this->end("Adapt");

    this->start("Verify");
    bool verify_result = ch.Verify(pk, m_p, h, r_p, pp);
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
	CH_FS_ECC_CCT_2024,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}




