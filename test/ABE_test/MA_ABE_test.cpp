#include "ABE/MA_ABE.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    int authNum;
};

class ABE_Test : public testing::TestWithParam<TestParams>{
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

TEST_P(ABE_Test, Test){
    MA_ABE abe(GetParam().curve);

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    // attribute
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<MA_ABE_pkTheta *> pkThetas;
    std::vector<MA_ABE_skTheta *> skThetas;
    std::vector<MA_ABE_skgidA *> skgidAs;

    MA_ABE_gpk gpk;

    MA_ABE_ciphertext c;


    element_s *msg = abe.GetGTElement();
    element_s *res = abe.GetGTElement();
    element_s *rt = abe.GetZrElement();

    this->start("GlobalSetup");
    abe.GlobalSetup(gpk);
    this->end("GlobalSetup");
    // Logger::PrintPbc("gpk.g", gpk.getElement("g"));

    for(int i = 0; i < GetParam().authNum; i++) {
        MA_ABE_pkTheta *pkTheta = new MA_ABE_pkTheta();
        MA_ABE_skTheta *skTheta = new MA_ABE_skTheta();

        if(i < SIZE_OF_ATTRIBUTES){
            this->start("AuthSetup");
            abe.AuthSetup(*pkTheta, *skTheta, gpk, ATTRIBUTES[i]);
            this->end("AuthSetup");    
        }else{
            this->start("AuthSetup");
            abe.AuthSetup(*pkTheta, *skTheta, gpk, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
            this->end("AuthSetup");
        }
        // Logger::PrintPbc("pkTheta.pkTheta_1", pkTheta->getElement("pkTheta_1"));
        // Logger::PrintPbc("pkTheta.pkTheta_2", pkTheta->getElement("pkTheta_2"));
        // Logger::PrintPbc("skTheta.aTheta", skTheta->getElement("aTheta"));
        // Logger::PrintPbc("skTheta.yTheta", skTheta->getElement("yTheta"));

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);
    }

    for(int i = 0; i < GetParam().authNum; i++) {
        MA_ABE_skgidA *skgidA = new MA_ABE_skgidA();

        if(i < SIZE_OF_ATTRIBUTES){
            this->start("KeyGen");
            abe.KeyGen(*skgidA, gpk, *skThetas[i], GID, ATTRIBUTES[i]);
            this->end("KeyGen");
        }else{
            this->start("KeyGen");
            abe.KeyGen(*skgidA, gpk, *skThetas[i], GID, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
            this->end("KeyGen");
        }
        // Logger::PrintPbc("skgidA.skgidA_0", skgidA->getElement("skgidA_0"));
        // Logger::PrintPbc("skgidA.skgidA_1", skgidA->getElement("skgidA_1"));

        skgidAs.push_back(skgidA);
    }

    Logger::PrintPbc("msg", msg);
    Logger::PrintPbc("rt", rt);

    this->start("Encrypt");
    abe.Encrypt(c, msg, rt, gpk, pkThetas, POLICY);
    this->end("Encrypt");
    // Logger::PrintPbc("c0", c.getC0()->getElement("c0"));
    // for(int i = 0;i < SIZE_OF_POLICY;i++){
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_1", c.getCi(i)->getElement("ci_1"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_2", c.getCi(i)->getElement("ci_2"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_3", c.getCi(i)->getElement("ci_3"));
    //     Logger::PrintPbc("ci[" + std::to_string(i) + "].ci_4", c.getCi(i)->getElement("ci_4"));
    // }
    
    // choose partial secret keys
    std::vector<MA_ABE_skgidA *> _skgidAs;  
    _skgidAs.push_back(skgidAs[0]);
    _skgidAs.push_back(skgidAs[1]);
    _skgidAs.push_back(skgidAs[2]);
    
    this->start("Decrypt");
    abe.Decrypt(res, _skgidAs, c);
    this->end("Decrypt");

    Logger::PrintPbc("msg", msg);
    Logger::PrintPbc("res", res);
    ASSERT_TRUE(element_cmp(msg, res) == 0);
}

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            test_params.push_back({curve, authNum});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	MA_ABE,
	ABE_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}