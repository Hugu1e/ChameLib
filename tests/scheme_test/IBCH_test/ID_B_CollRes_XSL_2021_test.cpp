#include <scheme/IBCH/ID_B_CollRes_XSL_2021.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    ID_B_CollRes_XSL_2021 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    ID_B_CollRes_XSL_2021_pp pp;
    ID_B_CollRes_XSL_2021_msk msk;
    ID_B_CollRes_XSL_2021_tk tk;
    ID_B_CollRes_XSL_2021_h h,h_p;

    element_t m,m_p;  // message
    element_t I;  // identity

    element_init_same_as(m, test.get_Zn());  
    element_init_same_as(m_p, test.get_Zn()); 
    element_init_same_as(I, test.get_Zn()); 

    element_random(m);
    element_random(I);
    element_random(m_p);

    unsigned long int m_bit_length = element_length_in_bytes(m) * 8;
    std::cout << "Bit length of m: " << m_bit_length << std::endl;

    test.start("SetUp");
    ch.SetUp(pp, msk, tk, h, h_p, m_bit_length);
    test.end("SetUp");
    // pp.print();
    msk.print();

    Logger::PrintPbc("I", I);
    test.start("KeyGen");
    ch.KeyGen(tk, msk, I, pp);
    test.end("KeyGen");
    tk.print();

    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, m, I, pp);
    test.end("Hash");
    h.get_h().print();
    h.get_r().print();

    test.start("Check");
    bool check_result = ch.Check(h, m, I, pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(h_p, m_p, h, m, tk);
    test.end("Adapt");
    h_p.get_h().print();
    h_p.get_r().print();
    
    test.start("Verify");
    bool verify_result = ch.Verify(h_p, m_p, I, pp);
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

