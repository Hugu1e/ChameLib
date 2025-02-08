#include <scheme/CH/CR_CH_DSS_2020.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CR_CH_DSS_2020 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CR_CH_DSS_2020_pp pp;
    CR_CH_DSS_2020_pk pk;
    CR_CH_DSS_2020_sk sk;
    CR_CH_DSS_2020_h h;
    CR_CH_DSS_2020_r r,r_p;

    element_t m,m_p;  // message
    element_init_same_as(m, test.get_G1());
    element_init_same_as(m_p, test.get_G1());

    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(pp, pk, sk, h, r, r_p);
    test.end("SetUp");
    pp.print();

    test.start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    test.end("KeyGen");
    pk.print();
    sk.print();

    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, r, m, pk, pp);
    test.end("Hash");
    h.print();
    r.print();

    test.start("Check");
    bool check_result = ch.Check(h, r, m, pk, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, h, r, m, m_p, sk, pk, pp);
    test.end("Adapt");
    r_p.print();

    test.start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, pk, pp);
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

