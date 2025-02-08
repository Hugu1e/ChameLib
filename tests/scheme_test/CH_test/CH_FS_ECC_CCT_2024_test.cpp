#include <CommonTest.h>
#include <scheme/CH/CH_FS_ECC_CCT_2024.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_FS_ECC_CCT_2024 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CH_FS_ECC_CCT_2024_pp pp;
    CH_FS_ECC_CCT_2024_pk pk;
    CH_FS_ECC_CCT_2024_sk sk;
    CH_FS_ECC_CCT_2024_r r,r_p;
    element_t m,m_p;
    element_t h;

    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_init_same_as(h, test.get_G1());

    element_random(m);
    element_random(m_p);

    test.start("SetUp");
    ch.SetUp(pp, pk, sk, r, r_p);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(pk, sk, pp);
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(h, r, pk, m, pp);
    test.end("Hash");
    r.print(c1);
    r.print(z1);
    r.print(z2);
    Logger::PrintPbc("h", h);

    test.start("Check");
    bool check_result = ch.Check(pk, m, h, r, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    test.start("Adapt");
    ch.Adapt(r_p, pk, sk, m, m_p, h, r, pp);
    test.end("Adapt");

    test.start("Verify");
    bool verify_result = ch.Verify(pk, m_p, h, r_p, pp);
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

