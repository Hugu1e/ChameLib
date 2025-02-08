#include <scheme/CH/CH_KEF_NoMH_AM_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_NoMH_AM_2004 ch = CH_KEF_NoMH_AM_2004(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CH_KEF_NoMH_AM_2004_pk pk;
    CH_KEF_NoMH_AM_2004_sk sk;
    element_t m,m_p;
    element_t r,r_p,s,s_p;
    element_t h;

    element_init_same_as(m, test.get_G1());
    element_init_same_as(m_p, test.get_G1());
    element_init_same_as(r, test.get_G1());
    element_init_same_as(r_p, test.get_G1());
    element_init_same_as(s, test.get_Zn());
    element_init_same_as(s_p, test.get_Zn());
    element_init_same_as(h, test.get_G1());

    test.start("Setup");
    ch.Setup(pk, sk);
    test.end("Setup");

    test.start("KeyGen");
    ch.KeyGen(pk, sk);
    test.end("KeyGen");
    pk.print();
    sk.print();

    element_random(m);
    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, r, s, pk, m);
    test.end("Hash");
    Logger::PrintPbc("Hash value", h);
    Logger::PrintPbc("r", r);
    Logger::PrintPbc("s", s);

    test.start("Check");
    bool check_result = ch.Check(pk, m, r, s, h);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    element_random(m_p);
    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, s_p, pk, sk, m_p, h);
    test.end("Adapt");
    Logger::PrintPbc("r_p", r_p);
    Logger::PrintPbc("s_p", s_p);

    test.start("Verify");
    bool verify_result = ch.Verify(pk, m_p, r_p, s_p, h);
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

