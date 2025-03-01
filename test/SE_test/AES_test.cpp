#include "SE/AES.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"
#include "curve/params.h"
#include "exception/CurveException.h"
#include "utils/RandomGenerator.h"

struct TestParams{
	int curve;
};

class SE_Test : public testing::TestWithParam<TestParams>{
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
            fprintf(out, "%s %s \n", testName.c_str(), curveName.c_str());
            printf("%s %s \n", testName.c_str(), curveName.c_str());
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

void initPairing(pairing_t pairing, std::string param) {
    pbc_param_t par;
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);
}

void initCurve(pairing_t pairing, const int curve){
    CurveParams curves;

    switch(curve){
        case Curve::A:
            initPairing(pairing, curves.a_param);
            break;
        case Curve::A1:
            initPairing(pairing, curves.a1_param);
            break;
        case Curve::E:
            initPairing(pairing, curves.e_param);
            break;
        case Curve::I:
            initPairing(pairing, curves.i_param);
            break;
        case Curve::A_80:
            initPairing(pairing, curves.a_param_80);
            break;
        case Curve::A_112:
            initPairing(pairing, curves.a_param_112);
            break;
        case Curve::A_128:
            initPairing(pairing, curves.a_param_128);
            break;
        case Curve::A_160:
            initPairing(pairing, curves.a_param_160);
            break;
        case Curve::SM9:
            initPairing(pairing, curves.sm9_param);
            break;
        case Curve::D_159:
            initPairing(pairing, curves.d159_param);
            break;
        case Curve::D_201:
            initPairing(pairing, curves.d201_param);
            break;
        case Curve::D_224:
            initPairing(pairing, curves.d224_param);
            break;
        case Curve::D_105171_196_185:
            initPairing(pairing, curves.d105171_196_185_param);
            break;
        case Curve::D_277699_175_167:
            initPairing(pairing, curves.d277699_175_167_param);
            break;
        case Curve::D_278027_190_181:
            initPairing(pairing, curves.d278027_190_181_param);
            break;
        case Curve::F:
            initPairing(pairing, curves.f_param);
            break;
        case Curve::G_149:
            initPairing(pairing, curves.g149_param);
            break;
        default:
            throw CurveException(CurveException::INVALID_CURVE);
    }
}

TEST_P(SE_Test, Test){
    AES aes;
    element_t key;
    mpz_t plaintext;
    mpz_t ciphertext;
    mpz_t decrypted_plaintext;

    pairing_t pairing;
    initCurve(pairing, GetParam().curve);

    element_init_GT(key, pairing);
    mpz_inits(plaintext, ciphertext, decrypted_plaintext, NULL);

    this->start("KGen");
    aes.KGen(key, 256);
    this->end("KGen");
    Logger::PrintPbc("key", key);

    this->start("Encrypt");
    RandomGenerator::RandomInLength(plaintext, 128);
    Logger::PrintGmp("plaintext", plaintext);
    aes.Enc(ciphertext, key, plaintext);
    this->end("Encrypt");

    this->start("Decrypt");
    aes.Dec(decrypted_plaintext, key, ciphertext);
    Logger::PrintGmp("decrypted_plaintext", decrypted_plaintext);
    this->end("Decrypt");

    bool result = mpz_cmp(plaintext, decrypted_plaintext) == 0;
    
    element_clear(key);
    mpz_clears(plaintext, ciphertext, decrypted_plaintext, NULL);
    ASSERT_TRUE(result);
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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	AES,
	SE_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}