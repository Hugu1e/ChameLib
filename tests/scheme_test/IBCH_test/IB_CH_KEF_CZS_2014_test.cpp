#include <scheme/IBCH/IB_CH_KEF_CZS_2014.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    IB_CH_KEF_CZS_2014 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    IB_CH_KEF_CZS_2014_pp pp;
    IB_CH_KEF_CZS_2014_td td;
    IB_CH_KEF_CZS_2014_h h;
    IB_CH_KEF_CZS_2014_r r,r_p;

    element_t m,m_p;  // message
    std::string ID = "identity string ID";
    std::string L = "customozed identity L";

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());

    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(&pp);
    test.end("SetUp");
    pp.printElement("P");
    pp.printElement("Ppub");

    test.start("Extract");
    ch.Extract(&td, ID);
    test.end("Extract");
    td.printElement("SID");


    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(&h, &r, ID, L, m, &pp);
    test.end("Hash");
    h.printElement("h");
    r.printElement("r1");
    r.printElement("r2");


    test.start("Check");
    bool check_result = ch.Check(&h, &r, L, m, &td);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(&r_p, m_p, &h, &r, L, m, &td);
    test.end("Adapt");
    r_p.printElement("r1");
    r_p.printElement("r2");


    test.start("Verify");
    bool verify_result = ch.Verify(&h, &r_p, L, m_p, &td);
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

