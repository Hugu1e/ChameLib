#include <CommonTest.h>

CommonTest::CommonTest(std::string test_name, std::string curve, bool out_file, bool visiable): out_file(out_file), visiable(visiable) {
    if(curve == "a") init_type(curves.a_param);
    else if(curve == "a80") init_type(curves.a_param_80);
    else if(curve == "a112") init_type(curves.a_param_112);
    else if(curve == "a128") init_type(curves.a_param_128);
    else if(curve == "a160") init_type(curves.a_param_160);
    else if(curve == "a1") init_type(curves.a1_param);
    else if(curve == "e") init_type(curves.e_param);
    else if(curve == "i") init_type(curves.i_param);
    else if(curve == "f") init_type(curves.f_param);
    else if(curve == "d224") init_type(curves.d224_param);
    else throw std::invalid_argument("CommonTest(): Invalid curve type");

    std::string filename = test_name + ".txt";
    out = fopen(filename.c_str(), "w");
    fflush(out);
}

void CommonTest::init_type(std::string param) {
    pbc_param_init_set_str(par, param.c_str());
    pairing_init_pbc_param(pairing, par);

    element_init_G1(G1, pairing);
    element_init_G2(G2, pairing);
    element_init_GT(GT, pairing);
    element_init_Zr(Zn, pairing);
}

void CommonTest::OutTime(std::string name, int id, double us) {
    us /= 1000;
    if(out_file) fprintf(out, "%s %d time: %lf ms.\n", name.c_str(), id, us);
    if(visiable) printf("%s %d time: %lf ms.\n", name.c_str(), id, us);
}

void CommonTest::start() {
    ts = std::chrono::system_clock::now();
}

void CommonTest::end(std::string test_name) {
    te = std::chrono::system_clock::now();
    OutTime(test_name, 0, std::chrono::duration_cast<std::chrono::microseconds>(te - ts).count());
}

CommonTest::~CommonTest() {
    fclose(out);
    
    element_clear(G1);
    element_clear(G2);
    element_clear(GT);
    element_clear(Zn);
}