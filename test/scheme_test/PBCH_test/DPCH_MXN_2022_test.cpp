#include "scheme/PBCH/DPCH_MXN_2022.h"
#include <gtest/gtest.h>
#include <stack>
#include <chrono>
#include "utils/Logger.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class PBCH_Test : public testing::TestWithParam<TestParams>{
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
            fprintf(out, "%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
            printf("%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
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

TEST_P(PBCH_Test, Test){
    DPCH_MXN_2022 ch(GetParam().curve);

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

    std::vector<DPCH_MXN_2022_pkTheta *> pkThetas;
    std::vector<DPCH_MXN_2022_skTheta *> skThetas;
    std::vector<DPCH_MXN_2022_skGidA *> skGidAs;

    DPCH_MXN_2022_pp ppDPCH;
    DPCH_MXN_2022_pk pkDPCH;
    DPCH_MXN_2022_sk skDPCH;
    DPCH_MXN_2022_skGid skGid;
    DPCH_MXN_2022_sigmaGid sigmaGid;
    DPCH_MXN_2022_h h;
    DPCH_MXN_2022_r r,r_p;
    std::string m = "message";
    std::string m_p = "message_p";

    this->start("SetUp");
    ch.SetUp(ppDPCH, pkDPCH, skDPCH, h, r, r_p, sigmaGid, skGid, GetParam().lamuda);
    this->end("SetUp");
    ppDPCH.get_gpk_MA_ABE().print();
    pkDPCH.get_pk_CH().print();
    skDPCH.get_sk_CH().print();
    pkDPCH.get_pk_BLS().print();
    skDPCH.get_sk_BLS().print();


    this->start("ModSetUp");
    ch.ModSetUp(skGid, sigmaGid, skDPCH, GID);
    this->end("ModSetUp");
    skGid.get_sk_CH().print();
    sigmaGid.get_signature().print();

    this->start("AuthSetUp");
    for(int i = 0; i < GetParam().authNum; i++) {
        DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
        DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;

        
        if(i < SIZE_OF_ATTRIBUTES){
            ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[i]);
        }else{
            ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
        }
        

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);

        // pkTheta->get_pk().print();
        // skTheta->get_sk().print();
    }
    this->end("AuthSetUp");

    this->start("ModKeyGen");
    for(int i = 0; i < GetParam().authNum; i++) {
        DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;

        
        if (i < SIZE_OF_ATTRIBUTES) {
            ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], ATTRIBUTES[i]);
        } else {
            ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
        }
        

        skGidAs.push_back(skGidA);

        // printf("skGidA->gid: %s\n", skGidA->get_sk().get_gid().c_str());
        // printf("skGidA->A: %s\n", skGidA->get_sk().get_A().c_str());
        // skGidA->get_sk().print();
    }
    this->end("ModKeyGen");

    this->start("Hash");
    ch.Hash(h, r, m, ppDPCH, pkDPCH, pkThetas, POLICY);
    this->end("Hash");
    // h0, h1, r0, r1
    h.get_h().print();
    r.get_r().print();

    this->start("Check");
    bool check_result = ch.Check(pkDPCH, m, h, r);
    this->end("Check");
    ASSERT_TRUE(check_result);

    // choose partial secret keys
    std::vector<DPCH_MXN_2022_skGidA *> _skGidAs;
    _skGidAs.push_back(skGidAs[0]);
    _skGidAs.push_back(skGidAs[1]);
    _skGidAs.push_back(skGidAs[2]);

    this->start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pkDPCH, skGid, _skGidAs, ppDPCH, pkThetas, POLICY);
    this->end("Adapt");
    // r0, r1
    r_p.get_r().print();


    this->start("Verify");
    bool verify_result = ch.Verify(pkDPCH, m_p, h, r_p);
    this->end("Verify");
    ASSERT_TRUE(verify_result);
}

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};
    int lamudas[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            for (int lamuda : lamudas) {
                test_params.push_back({curve, authNum, lamuda});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	DPCH_MXN_2022,
	PBCH_Test,
	testing::ValuesIn(test_values)
);

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}