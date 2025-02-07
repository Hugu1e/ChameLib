#include <scheme/CH/CH_AMV_2017.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_AMV_2017 ch = CH_AMV_2017(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CH_AMV_2017_pk pk;
    CH_AMV_2017_sk sk;
    CH_AMV_2017_h h,h_p;
    element_t m,m_p;

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());


    test.start("KeyGen");
    ch.KeyGen(&pk, &sk);
    test.end("KeyGen");
    Logger::PrintPbcElements("pk_ch", *pk.get_CH_pk());
    Logger::PrintPbcElements("sk_ch", *sk.get_CH_sk());


    element_random(m);
    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(&h, m, &pk);
    test.end("Hash");
    Logger::PrintPbc("Hash value", h.get_h()->getElement("h"));


    test.start("Check");
    bool check_result = ch.Check(&h, m, &pk);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    element_random(m_p);
    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(&h_p, m_p, &h, m, &sk, &pk);
    test.end("Adapt");
    // Logger::PrintPbcElements("h_p.h", *h_p.get_h());
    // Logger::PrintPbcElements("h_p.r.c1", *h_p.get_r()->get_c1());
    // Logger::PrintPbcElements("h_p.r.c2", *h_p.get_r()->get_c2());
    // Logger::PrintPbcElements("h_p.r.pai", *h_p.get_r()->get_pai());



    test.start("Verify");
    bool verify_result = ch.Verify(&h_p, m_p, &pk);
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

