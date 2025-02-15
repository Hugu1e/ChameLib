#ifndef CHAMELIB_PKE_CPA_AMV_2017_H
#define CHAMELIB_PKE_CPA_AMV_2017_H

#include "../base/PbcScheme.h"
#include "../base/PbcElements.h"
#include "../utils/Hash.h"
#include "../utils/TypeConverter.h"

class PKE_CPA_AMV_2017_pk:public PbcElements{};

class PKE_CPA_AMV_2017_sk:public PbcElements{};

class PKE_CPA_AMV_2017_c:public PbcElements{};

class PKE_CPA_AMV_2017: public PbcScheme{
    public:
        PKE_CPA_AMV_2017(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void Setup(PKE_CPA_AMV_2017_pk &pk, PKE_CPA_AMV_2017_sk &sk,PKE_CPA_AMV_2017_c &c);

        void KeyGen(PKE_CPA_AMV_2017_pk &pk, PKE_CPA_AMV_2017_sk &sk);

        void Encrypt(PKE_CPA_AMV_2017_c &c, element_t m, PKE_CPA_AMV_2017_pk &pk);

        void Decrypt(element_t m, PKE_CPA_AMV_2017_c &c, PKE_CPA_AMV_2017_sk &sk);

        ~PKE_CPA_AMV_2017();

        enum {
            x
        };
        
        enum {
            g,
            y
        };
        
        enum {
            rho,
            c1,
            c2,
            c2_
        };
};

#endif  // CHAMELIB_PKE_CPA_AMV_2017_H