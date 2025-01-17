#ifndef CHAMELIB_BLS_H
#define CHAMELIB_BLS_H

#include <base/PbcElements.h>
#include <utils/Hash.h>

class BLS_pp: public PbcElements{};
class BLS_pk: public PbcElements{};
class BLS_sk: public PbcElements{};
class BLS_signature: public PbcElements{};

class BLS{
    private:
        element_s *G1, *G2, *GT, *Zn;

    public:
        BLS(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void Setup(BLS_pp *pp);
        void Setup(BLS_pp *pp, element_t g);

        void KeyGen(BLS_pk *pk, BLS_sk *sk, BLS_pp *pp);

        void H(element_t res, std::string m);

        void Sign(BLS_signature *signature, BLS_sk *sk, std::string message);

        bool Verify(BLS_pp *pp, BLS_pk *pk, std::string message, BLS_signature *signature);

        ~BLS();
        
};

#endif  // CHAMELIB_BLS_H