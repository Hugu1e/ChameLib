#include <CommonTest.h>
#include <scheme/CH/CH_ET_KOG_CDK_2017.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    CH_ET_KOG_CDK_2017 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    CH_ET_KOG_CDK_2017_pp pp;
    CH_ET_KOG_CDK_2017_sk sk;
    CH_ET_KOG_CDK_2017_pk pk;
    CH_ET_KOG_CDK_2017_h h;
    CH_ET_KOG_CDK_2017_r r, r_p;
    CH_ET_KOG_CDK_2017_etd etd;

    element_t m,m_p;
    element_init_same_as(m, test.get_Zn());
    element_init_same_as(m_p, test.get_Zn());
    element_random(m);
    element_random(m_p);

    // // Test NIZK
    // element_t z, R, pk, sk, g;
    // element_init_same_as(z, test.get_Zn());
    // element_init_same_as(R, test.get_G1());
    // element_init_same_as(pk, test.get_G1());
    // element_init_same_as(sk, test.get_Zn());
    // element_init_same_as(g, test.get_G1());

    // element_random(sk);
    // element_random(g);
    // // pk = g^sk
    // element_pow_zn(pk, g, sk);

    // ch.NIZKPOK_proof(z, R, pk, sk, g);
    // if(ch.NIZKPOK_verify(z, R, pk, g)){
    //     printf("NIZKPOK_verify success\n");
    // }else{
    //     printf("NIZKPOK_verify fail\n");
    // }

    test.start("SetUp");
    ch.SetUp(pp, sk, pk, h, etd, r, r_p);
    test.end("SetUp");

    test.start("KeyGen");
    ch.KeyGen(sk, pk, pp);
    test.end("KeyGen");

    test.start("Hash");
    ch.Hash(h, r, etd, m, pk, pp);
    test.end("Hash");
    h.get_hash().print();
    r.get_ch_r().print();
    r.get_enc_c().print();

    test.start("Check");
    bool check = ch.Check(h, m, r, pk, pp);
    test.end("Check");

    if(check){
        printf("Hash check success\n");
    }else{
        printf("Hash check failed\n");
    }

    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, pk, pp, sk, etd);
    test.end("Adapt");
    r_p.get_ch_r().print();
    r_p.get_enc_c().print();

    test.start("Verify");
    bool verify = ch.Verify(h, m_p, r_p, pk, pp);
    test.end("Verify");

    if(verify){
        printf("Verify success\n");
        test_result = 0;
    }else{
        printf("Verify failed\n");
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

