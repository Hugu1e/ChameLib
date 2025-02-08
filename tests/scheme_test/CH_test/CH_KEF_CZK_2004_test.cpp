#include <scheme/CH/CH_KEF_CZK_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_CZK_2004 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());
    CH_KEF_CZK_2004_pp pp;
    CH_KEF_CZK_2004_pk pk;
    CH_KEF_CZK_2004_sk sk;
    CH_KEF_CZK_2004_r r,r_p;
    element_t I;  // identity
    element_t h;
    element_t m,m_p;

    element_init_same_as(I, test.get_G1());
    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_init_same_as(h, test.get_G1());

    element_random(I);
    element_random(m);
    element_random(m_p);


    test.start("SetUp");
    ch.SetUp(pp, pk, sk, r, r_p);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    test.end("KeyGen");

    Logger::PrintPbc("m", m);
    Logger::PrintPbc("I", I);
    test.start("Hash");
    ch.Hash(h, r, m, I, pk, pp);
    test.end("Hash");
    Logger::PrintPbc("h", h);
    r.print();

    test.start("Check");
    bool check_result = ch.Check(I, m, r, h, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, sk, h, m, r, m_p, I, pp);
    test.end("Adapt");
    r_p.print();

    test.start("Verify");
    bool verify_result = ch.Verify(I, m_p, r_p, h, pp);
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

