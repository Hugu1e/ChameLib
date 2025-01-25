#ifndef CH_KEF_NoMH_AM_2004_H
#define CH_KEF_NoMH_AM_2004_H

#include <utils/Hash.h>
#include <base/PbcScheme.h>
#include <base/PbcElements.h>

class CH_KEF_NoMH_AM_2004_pk : public PbcElements{};
class CH_KEF_NoMH_AM_2004_sk : public PbcElements{};

class CH_KEF_NoMH_AM_2004: public PbcScheme{
    public:
        CH_KEF_NoMH_AM_2004(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void KeyGen(CH_KEF_NoMH_AM_2004_pk *pk, CH_KEF_NoMH_AM_2004_sk *sk);

        void H(element_t res, element_t m1, element_t m2);

        void Hash(element_t h, element_t r, element_t s, CH_KEF_NoMH_AM_2004_pk *pk, element_t m);

        bool Check(CH_KEF_NoMH_AM_2004_pk *pk, element_t m, element_t r, element_t s, element_t h);

        void Adapt(element_t r_p,  element_t s_p, CH_KEF_NoMH_AM_2004_pk *pk, CH_KEF_NoMH_AM_2004_sk *sk, element_t m_p, element_t h);
        
        bool Verify(CH_KEF_NoMH_AM_2004_pk *pk, element_t m_p, element_t r_p, element_t s_p, element_t h);

        ~CH_KEF_NoMH_AM_2004();
};


#endif  // CH_KEF_NoMH_AM_2004_H