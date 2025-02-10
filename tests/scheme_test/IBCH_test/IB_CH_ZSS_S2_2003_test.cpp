#include <scheme/IBCH/IB_CH_ZSS_S2_2003.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    IB_CH_ZSS_S2_2003 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    IB_CH_ZSS_S2_2003_pp pp;
    IB_CH_ZSS_S2_2003_msk msk;
    IB_CH_ZSS_S2_2003_sk sk;
    IB_CH_ZSS_S2_2003_h h;
    IB_CH_ZSS_S2_2003_r r,r_p;

    element_t m,m_p;  // message
    element_t ID;

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_init_same_as(ID, test.get_Zn());

    element_random(m);
    element_random(m_p);
    element_random(ID);

    test.start("SetUp");
    ch.SetUp(pp, msk, sk, h, r, r_p);
    test.end("SetUp");
    pp.print();
    msk.print();

    Logger::PrintPbc("ID", ID);
    test.start("Extract");
    ch.Extract(sk, msk, ID, pp);
    test.end("Extract");
    sk.print();


    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, r, m, ID, pp);
    test.end("Hash");
    // h
    h.print();
    // r
    r.print();


    test.start("Check");
    bool check_result = ch.Check(h, m, r, ID, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, ID, sk, pp);
    test.end("Adapt");
    // r_p
    r_p.print();


    test.start("Verify");
    bool verify_result = ch.Verify(h, m_p, r_p, ID, pp);
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

