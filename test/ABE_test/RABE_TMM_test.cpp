#include "ABE/RABE_TMM.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
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
            fprintf(out, "%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
            printf("%s %s swap: %d leafNodeSize: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap, GetParam().leafNodeSize);
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
    RABE_TMM abe(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    const int SIZE_OF_POLICY = 4;

    element_s *id1 = abe.GetZrElement();
    element_s *id2 = abe.GetZrElement();
    element_s *id3 = abe.GetZrElement();

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);

    RABE_TMM_mpk mpk;
    RABE_TMM_msk msk;
    RABE_TMM_skid skid1, skid2, skid3;
    RABE_TMM_kut kut;
    RABE_TMM_dkidt dkidt1, dkidt2, dkidt3;
    RABE_TMM_ciphertext ciphertext;

    std::vector<RABE_TMM_revokedPreson> rl;
    Binary_tree_RABE st;

    element_s *msg = abe.GetZrElement();
    element_s *res = abe.GetZrElement();
    element_s *s1 = abe.GetZrElement();
    element_s *s2 = abe.GetZrElement();

    this->start("Setup");
    abe.Setup(mpk, msk, rl, st, GetParam().leafNodeSize);
    this->end("Setup");
    // st->printTree();
    // Logger::PrintPbc("g", msk.getElement("g"));
    // Logger::PrintPbc("h", msk.getElement("h"));
    // Logger::PrintPbc("a1", msk.getElement("a1"));
    // Logger::PrintPbc("a2", msk.getElement("a2"));
    // Logger::PrintPbc("b1", msk.getElement("b1"));
    // Logger::PrintPbc("b2", msk.getElement("b2"));
    // Logger::PrintPbc("g_pow_d1", msk.getElement("g_pow_d1"));
    // Logger::PrintPbc("g_pow_d2", msk.getElement("g_pow_d2"));
    // Logger::PrintPbc("g_pow_d3", msk.getElement("g_pow_d3"));
    // Logger::PrintPbc("h", mpk.getElement("h"));
    // Logger::PrintPbc("H1", mpk.getElement("H1"));
    // Logger::PrintPbc("H2", mpk.getElement("H2"));
    // Logger::PrintPbc("T1", mpk.getElement("T1"));
    // Logger::PrintPbc("T2", mpk.getElement("T2"));

    this->start("KeyGen");
    abe.KGen(skid1, st, mpk, msk, attr_list, id1, re_time_1);
    this->end("KeyGen");
    abe.KGen(skid2, st, mpk, msk, attr_list, id2, re_time_2);
    abe.KGen(skid3, st, mpk, msk, attr_list, id3, re_time_3);
    // Logger::PrintPbc("sk0.sk_1", skid.get_sk0()->getElement("sk0_1"));
    // Logger::PrintPbc("sk0.sk_2", skid.get_sk0()->getElement("sk0_2"));
    // Logger::PrintPbc("sk0.sk_3", skid.get_sk0()->getElement("sk0_3"));
    // for(int i = 0;i < SIZE_OF_ATTR;i++){
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_1", skid.get_sk_y(i)->getElement("sk_1"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_2", skid.get_sk_y(i)->getElement("sk_2"));
    //     Logger::PrintPbc("sk_y[" + std::to_string(i) + "].sk_3", skid.get_sk_y(i)->getElement("sk_3"));
    // }
    // Logger::PrintPbc("sk_prime.sk_1", skid.get_sk_prime()->getElement("sk_1"));
    // Logger::PrintPbc("sk_prime.sk_2", skid.get_sk_prime()->getElement("sk_2"));
    // Logger::PrintPbc("sk_prime.sk_3", skid.get_sk_prime()->getElement("sk_3"));


    this->start("Rev");
    abe.Rev(rl, id1, re_time_1);
    this->end("Rev");
    abe.Rev(rl, id2, re_time_2);
    abe.Rev(rl, id3, re_time_3);

    Logger::PrintPbc("msg", msg);
    this->start("Encrypt");
    abe.Enc(ciphertext, mpk, msg, POLICY, T_present, s1, s2);
    this->end("Encrypt");
    // Logger::PrintPbc("ct0.ct_1", ciphertext.get_ct0()->getElement("ct0_1"));
    // Logger::PrintPbc("ct0.ct_2", ciphertext.get_ct0()->getElement("ct0_2"));
    // Logger::PrintPbc("ct0.ct_3", ciphertext.get_ct0()->getElement("ct0_3"));
    // for(int i = 0;i < SIZE_OF_POLICY;i++){
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_1", ciphertext.get_ct_y(i)->getElement("ct_1"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_2", ciphertext.get_ct_y(i)->getElement("ct_2"));
    //     Logger::PrintPbc("ct_y[" + std::to_string(i) + "].ct_3", ciphertext.get_ct_y(i)->getElement("ct_3"));
    // }
    // Logger::PrintPbc("ct_prime", ciphertext.get_ct_prime()->getElement("ct_prime"));

    this->start("KUpt");
    abe.KUpt(kut, mpk, st, rl, T_present);
    this->end("KUpt");
    printf("size of kut.ku_theta: %ld\n", kut.get_ku_theta().size());

    this->start("DKGen");
    try{
        abe.DKGen(dkidt1, mpk, skid1, kut);
    }catch(const std::runtime_error& e){
        printf("%s\n", e.what());
    }
    this->end("DKGen");

    this->start("DKGen");
    try{
        abe.DKGen(dkidt3, mpk, skid3, kut);
    }catch(const std::runtime_error& e){
        printf("%s\n", e.what());
    }
    this->end("DKGen");

    this->start("DKGen");
    abe.DKGen(dkidt2, mpk, skid2, kut);
    this->end("DKGen");

    this->start("Decrypt");
    abe.Dec(res, mpk, ciphertext, dkidt2);
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

    int leadNodeSizes[] = {8, 16, 32, 64};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                test_params.push_back({curve, swap, leafNodeSize});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	RABE_TMM,
	ABE_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}