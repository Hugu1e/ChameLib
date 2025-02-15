#ifndef CHAMELIB_CHTEST_H
#define CHAMELIB_CHTEST_H

#include <CommonTest.h>

template <typename CH,typename PP = CH*, typename PK, typename SK, typename H, typename M, typename M_P, typename R, typename R_P, typename ETD>
class CHTest{
    private:
        CH ch;
        PP pp;
        PK pk;
        SK sk;
        H h;
        M m;
        M_P m_p;
        R r;
        R_P r_p;
        ETD etd;

        int test_result = 1;
    
    public:
        CHTest(CH ch, PP pp = nullptr, PK pk, SK sk, H h, M m, M_P m_p, R r, R_P r_p, ETD etd){
            this->ch = ch;
            this->pp = pp;
            this->pk = pk;
            this->sk = sk;
            this->h = h;
            this->m = m;
            this->m_p = m_p;
            this->r = r;
            this->r_p = r_p;
            this->etd = etd;
        }

        int test(std::string test_name, std::string curve){
            CommonTest test(test_name, curve);

        }

};

#endif  // CHAMELIB_CHTEST_H
