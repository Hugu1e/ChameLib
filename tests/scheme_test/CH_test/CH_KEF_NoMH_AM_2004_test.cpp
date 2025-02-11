#include <scheme/CH/CH_KEF_NoMH_AM_2004.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_KEF_NoMH_AM_2004 ch = CH_KEF_NoMH_AM_2004();

    CH_KEF_NoMH_AM_2004_pk pk;
    CH_KEF_NoMH_AM_2004_sk sk;
    CH_KEF_NoMH_AM_2004_h h;
    CH_KEF_NoMH_AM_2004_r r, r_p;

    mpz_t m,m_p;
    mpz_inits(m, m_p, NULL);
    RandomGenerator::RandomInLength(m, 1024);
    RandomGenerator::RandomInLength(m_p, 1024);


    test.start("Setup");
    ch.Setup(pk, sk, h, r, r_p);
    test.end("Setup");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, 512);
    test.end("KeyGen");
    pk.print();
    sk.print();

    Logger::PrintGmp("m", m);
    test.start("Hash");
    ch.Hash(h, r, m, pk);
    test.end("Hash");
    h.print();
    r.print();


    test.start("Check");
    bool check_result = ch.Check(h, r, m, pk);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintGmp("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, r, m, pk, sk);
    test.end("Adapt");
    r_p.print();

    test.start("Verify");
    bool verify_result = ch.Verify(h, r_p, m_p, pk);
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

