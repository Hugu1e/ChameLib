#include <scheme/CH/CH_KEF_DLP_LLA_2012.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_DLP_LLA_2012 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CH_KEF_DLP_LLA_2012_pp pp;
    CH_KEF_DLP_LLA_2012_sk sk;
    CH_KEF_DLP_LLA_2012_pk pk;
    CH_KEF_DLP_LLA_2012_label label;
        
    element_t m, m_p, m_pp;
    element_t r, r_p, r_pp;
    element_t h;

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_init_same_as(m_pp, test.get_Zn());
    element_init_same_as(r, test.get_Zn());
    element_init_same_as(r_p, test.get_Zn());
    element_init_same_as(r_pp, test.get_Zn());
    element_init_same_as(h, test.get_G1());

    element_random(m);
    element_random(m_p);
    element_random(m_pp);
    Logger::PrintPbc("m", m);
    Logger::PrintPbc("m_p", m_p);
    Logger::PrintPbc("m_pp", m_pp);


    test.start("SetUp");
    ch.SetUp(pp, pk, sk, label);
    test.end("SetUp");
    // pp.printElement("g");

    test.start("KeyGen");
    ch.KeyGen(sk, pk, label, pp);
    test.end("KeyGen");
    // sk.printElement("a");
    // sk.printElement("x1");
    // sk.printElement("x2");

    test.start("Hash");
    ch.Hash(h, r, pk, m, label, pp);
    test.end("Hash");
    Logger::PrintPbc("Hash value", h);
    Logger::PrintPbc("r", r);

    test.start("Check");
    bool check_result = ch.Check(m, r, pk, label, h, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    test.start("UForge");
    ch.UForge(r_p, sk, pk, label, h, m, m_p, r, pp);
    test.end("UForge");
    Logger::PrintPbc("r_p", r_p);

    test.start("Verify");
    bool verify_result_1 = ch.Verify(m_p, r_p, pk, label, h, pp);
    test.end("Verify");

    test.start("IForge");
    ch.IForge(r_pp, label, m, m_p, r, r_p, m_pp);
    test.end("IForge");
    Logger::PrintPbc("r_pp", r_pp);

    test.start("Verify");
    bool verify_result_2 = ch.Verify(m_pp, r_pp, pk, label, h, pp);
    test.end("Verify");

    if(verify_result_1){
        printf("Verify_UForge successful!\n");
    }else{
        printf("Verify_UForge failed.\n");
    }

    if(verify_result_2){
        printf("Verify_IForge successful!\n");
    }else{
        printf("Verify_IForge failed.\n");
    }

    if(verify_result_1 && verify_result_2){
        test_result = 0;
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

