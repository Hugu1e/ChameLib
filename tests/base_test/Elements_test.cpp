#include <base/PbcElements.h>
#include <CommonTest.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("usage: %s [a|e|i|f|d224]\n", argv[0]);
        return 0;
    }

    CommonTest test(argv[0], argv[1]);

    element_t m;
    element_init_same_as(m, test.get_Zn());  

    test.start();

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
}