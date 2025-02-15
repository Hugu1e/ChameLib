#include <scheme/CH/CH_KEF_MH_SDH_DL_AM_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_MH_SDH_DL_AM_2004 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());
        
    CH_KEF_MH_SDH_DL_AM_2004_pp pp;
    CH_KEF_MH_SDH_DL_AM_2004_sk sk;
    CH_KEF_MH_SDH_DL_AM_2004_pk pk;
    CH_KEF_MH_SDH_DL_AM_2004_h h;
    CH_KEF_MH_SDH_DL_AM_2004_r r,r_p;
    element_t label;
    element_t m,m_p;

    element_init_same_as(label, test.get_Zn()); 
    element_init_same_as(m, test.get_Zn());  
    element_init_same_as(m_p, test.get_Zn()); 

    element_random(label);
    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    test.end("KeyGen");

    Logger::PrintPbc("label", label);
    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, r, m, label, pk, pp);
    test.end("Hash");
    Logger::PrintPbc("Hash value", h[0]);
    Logger::PrintPbc("r", r[0]);

    test.start("Check");
    bool check_result = ch.Check(h, r, m, label, pk, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, label, sk, pp);
    test.end("Adapt");
    Logger::PrintPbc("r_p", r_p[0]);

    test.start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, label, pk, pp);
    test.end("Verify");

    if(verify_result){
        printf("Verify successful!\n");
        test_result = 0;
    }else{
        printf("Verify failed.\n");
    }
}


int main(int argc, char *argv[]){
    if(argc == 1) {
        test(argv[0], "a");
    }else if(argc == 2){
        test(argv[0], argv[1]);
    }else{
        printf("usage: %s [a|e|i|f|d224]\n", argv[0]);
        return 1;
    }
    return test_result;
}

