#include <scheme/CH_ET_BC_CDK_2017.h>

int test_result = 1;

CH_ET_BC_CDK_2017::pp pp;
CH_ET_BC_CDK_2017::pk pk;
CH_ET_BC_CDK_2017::sk sk;
CH_ET_BC_CDK_2017::etd etd;
CH_ET_BC_CDK_2017::h h;
CH_ET_BC_CDK_2017::r r,r_p;

string m = "123456";
string m_p = "789101";

void test_init(){

}

void test_clear(){
    
}


int main(int argc, char *argv[]){
    test_init();
    CH_ET_BC_CDK_2017 *test = new CH_ET_BC_CDK_2017();

    test->SetUp(&pp, 1024);

    test->KeyGen(&pk, &sk, &pp);

    test->Hash(&h, &r, &etd, &pp, &pk, m);
    PrintMpz("h0", h.h0);
    PrintMpz("h1", h.h1);

    if(test->Check(&h, &r, &pk, m)){
        printf("Hash check successful!\n");
    }else{
        printf("Hash check failed.\n");
    }

    test->Adapt(&r_p, &sk, &etd, &pk, &h, &r, m, m_p);
    PrintMpz("r0_p", r_p.r0);
    PrintMpz("r1_p", r_p.r1);   
    
    // Check
    printf("Verify:\n");
    if(test->Verify(&h, &r_p, &pk, m_p)){
        printf("Verify successful.\n");
        test_result = 0;
    }else{
        printf("Verify failed.\n");
    }

    test_clear();
    return test_result;
}