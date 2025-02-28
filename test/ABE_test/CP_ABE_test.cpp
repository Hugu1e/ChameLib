#include "ABE/CP_ABE.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    bool swap;
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
            fprintf(out, "%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            printf("%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
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
    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    CP_ABE abe(GetParam().curve, GetParam().swap);
    CP_ABE_mpk mpk;
    CP_ABE_msk msk;
    CP_ABE_sks sks;
    CP_ABE_ciphertext ciphertext;

    element_s* msg = abe.GetGTElement();
    element_s* res = abe.GetGTElement();
    
    this->start("Setup");
    abe.Setup(msk, mpk);
    this->end("Setup");
    // msk.printElement();
    // mpk.printElement();

    this->start("KeyGen");
    abe.KeyGen(sks, msk, mpk, attr_list);
    this->end("KeyGen");
    // Logger::PrintPbc("sk0.sk_1", sks.get_sk0()->getElement("sk_1"));
    // Logger::PrintPbc("sk0.sk_2", sks.get_sk0()->getElement("sk_2"));
    // Logger::PrintPbc("sk0.sk_3", sks.get_sk0()->getElement("sk_3"));
    // for(int i = 0;i < SIZE_OF_ATTR;i++){
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_1", sks.get_sk_y(i)->getElement("sk_1"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_2", sks.get_sk_y(i)->getElement("sk_2"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_3", sks.get_sk_y(i)->getElement("sk_3"));
    // }
    // Logger::PrintPbc("sk_prime.sk_1", sks.get_sk_prime()->getElement("sk_1"));
    // Logger::PrintPbc("sk_prime.sk_2", sks.get_sk_prime()->getElement("sk_2"));
    // Logger::PrintPbc("sk_prime.sk_3", sks.get_sk_prime()->getElement("sk_3"));

    Logger::PrintPbcWithSize("msg", msg);
    this->start("Encrypt");
    abe.Encrypt(ciphertext, mpk, msg, POLICY);
    this->end("Encrypt");
    Logger::PrintPbc("ct0.ct_1", ciphertext.get_ct_0()[CP_ABE::ct_1]);
    Logger::PrintPbc("ct0.ct_2", ciphertext.get_ct_0()[CP_ABE::ct_2]);
    Logger::PrintPbc("ct0.ct_3", ciphertext.get_ct_0()[CP_ABE::ct_3]);
    for(int i = 0;i < SIZE_OF_POLICY;i++){
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.get_ct_y(i)[CP_ABE::ct_1]);
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.get_ct_y(i)[CP_ABE::ct_2]);
        Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.get_ct_y(i)[CP_ABE::ct_3]);
    }
    Logger::PrintPbc("ct_prime", ciphertext.get_ct_prime()[0]);

    this->start("Decrypt");
    abe.Decrypt(res, ciphertext, mpk, sks);
    this->end("Decrypt");
    Logger::PrintPbc("msg", msg);
    Logger::PrintPbc("res", res);

    ASSERT_TRUE(element_cmp(msg, res) == 0);
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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CP_ABE,
	ABE_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}