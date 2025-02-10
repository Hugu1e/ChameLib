#include <scheme/IBCH/ID_B_CollRes_XSL_2021.h>
#include <CommonTest.h>

int test_result = 1;

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    ID_B_CollRes_XSL_2021 ch(test.get_G1(), test.get_G2(), test.get_GT(), test.get_Zn());

    ID_B_CollRes_XSL_2021_pp pp;
    ID_B_CollRes_XSL_2021_msk msk;
    ID_B_CollRes_XSL_2021_tk tk;
    ID_B_CollRes_XSL_2021_h h;
    ID_B_CollRes_XSL_2021_r r,r_p;

    element_t m,m_p;  // message
    std::string I = "IDENTITY";
    int n = I.length() * 8;
    std::cout << "Bit length of m: " << n << std::endl;

    element_init_same_as(m, test.get_Zn());  
    element_init_same_as(m_p, test.get_Zn()); 

    element_random(m);
    element_random(m_p);
    

    test.start("SetUp");
    ch.SetUp(pp, msk, tk, h, r, r_p, n);
    test.end("SetUp");
    // pp.print();
    msk.print();

    test.start("KeyGen");
    ch.KeyGen(tk, msk, I.c_str(), pp);
    test.end("KeyGen");
    tk.print();

    Logger::PrintPbc("m", m);
    test.start("Hash");
    ch.Hash(h, r, m, I.c_str(), pp);
    test.end("Hash");
    h.print();
    r.print();

    test.start("Check");
    bool check_result = ch.Check(h, m, r, I.c_str(), pp);
    test.end("Check");

    if(check_result){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    Logger::PrintPbc("m_p", m_p);
    test.start("Adapt");
    ch.Adapt(r_p, m_p, h, m, r, tk);
    test.end("Adapt");
    h.print();
    r.print();
    
    test.start("Verify");
    bool verify_result = ch.Verify(h, m_p, r_p, I.c_str(), pp);
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

