#include <base/PbcElements.h>
#include <base/MpzElements.h>
#include <CommonTest.h>

void test(std::string test_name, std::string curve){
    CommonTest test(test_name, curve);

    element_t m;
    element_init_same_as(m, test.get_Zn());  

    test.start("test1");

    element_set1(m);

    PbcElements pbcElements;
    pbcElements.printElement("m");

    pbcElements.insertElement("m", "Zn", m);
    // pbcElements.insertElement("m", "Zn", m);

    pbcElements.printElement("m");
    
    element_t m2;
    element_init_same_as(m2, m);
    element_set(m2, pbcElements.getElement("m"));
    element_printf("m: %B\n", m);
    element_printf("m2: %B\n", m2);

    test.end("test1");

    test.start("test2");
    mpz_t n;
    mpz_init(n);

    MpzElements mpzElements;
    mpzElements.printElement("n");

    mpz_set_ui(n, 1234567890);
    mpzElements.insertElement("n", n);
    mpzElements.printElement("n");
    mpzElements.printElement();

    mpz_t n2;
    mpz_init(n2);
    mpz_set(n2, mpzElements.getElement("n"));
    gmp_printf("n: %Zd\n", n);
    gmp_printf("n2: %Zd\n", n2);

    test.end("test2");
}

int main(int argc, char *argv[]) {
    if(argc == 1) {
        test(argv[0], "a");
    }else if(argc == 2){
        test(argv[0], argv[1]);
    }else{
        printf("usage: %s [a|e|i|f|d224]\n", argv[0]);
        return 1;
    }
}