#include "scheme/CH/CH_ET_KOG_CDK_2017.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>

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
    CH_ET_KOG_CDK_2017 ch(GetParam().curve, GetParam().group);

    CH_ET_KOG_CDK_2017_pp pp;
    CH_ET_KOG_CDK_2017_sk sk;
    CH_ET_KOG_CDK_2017_pk pk;
    CH_ET_KOG_CDK_2017_h h;
    CH_ET_KOG_CDK_2017_r r, r_p;
    CH_ET_KOG_CDK_2017_etd etd;

    // element_t m,m_p;
    // element_init_same_as(m, this->get_Zn());
    // element_init_same_as(m_p, this->get_Zn());
    // element_random(m);
    // element_random(m_p);
    element_s* m = ch.GetZrElement();
    element_s* m_p = ch.GetZrElement();

    // // Test NIZK
    // element_t z, R, pk, sk, g;
    // element_init_same_as(z, this->get_Zn());
    // element_init_same_as(R, this->get_G1());
    // element_init_same_as(pk, this->get_G1());
    // element_init_same_as(sk, this->get_Zn());
    // element_init_same_as(g, this->get_G1());

    // element_random(sk);
    // element_random(g);
    // // pk = g^sk
    // element_pow_zn(pk, g, sk);

    // ch.NIZKPOK_proof(z, R, pk, sk, g);
    // if(ch.NIZKPOK_verify(z, R, pk, g)){
    //     printf("NIZKPOK_verify success\n");
    // }else{
    //     printf("NIZKPOK_verify fail\n");
    // }

    this->start("SetUp");
    ch.SetUp(pp, sk, pk, h, etd, r, r_p);
    this->end("SetUp");

    this->start("KeyGen");
    ch.KeyGen(sk, pk, pp);
    this->end("KeyGen");

    this->start("Hash");
    ch.Hash(h, r, etd, m, pk, pp);
    this->end("Hash");
    h.get_hash().print();
    r.get_ch_r().print();
    r.get_enc_c().print();

    this->start("Check");
    bool check = ch.Check(h, m, r, pk, pp);
    this->end("Check");
    ASSERT_TRUE(check);

    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, pk, pp, sk, etd);
    this->end("Adapt");
    r_p.get_ch_r().print();
    r_p.get_enc_c().print();

    this->start("Verify");
    bool verify = ch.Verify(h, m_p, r_p, pk, pp);
    this->end("Verify");
    ASSERT_TRUE(verify);
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
	CH_ET_KOG_CDK_2017,
	CH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



