#ifndef CHAMELIB_PKE_CCA_AMV_2017_H
#define CHAMELIB_PKE_CCA_AMV_2017_H

#include <base/PbcScheme.h>
#include <base/PbcElements.h>
#include <utils/Hash.h>
#include <utils/TypeConverter.h>

class PKE_CCA_AMV_2017_pk:public PbcElements{};

class PKE_CCA_AMV_2017_sk:public PbcElements{};

class PKE_CCA_AMV_2017_c:public PbcElements{};

class PKE_CCA_AMV_2017: public PbcScheme{
    public:
        PKE_CCA_AMV_2017(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void KeyGen(PKE_CCA_AMV_2017_pk *pk, PKE_CCA_AMV_2017_sk *sk);

        void Encrypt(PKE_CCA_AMV_2017_c *c, element_t m, PKE_CCA_AMV_2017_pk *pk);

        void Decrypt(element_t m, PKE_CCA_AMV_2017_c *c, PKE_CCA_AMV_2017_sk *sk);

        ~PKE_CCA_AMV_2017();
};

#endif  // CHAMELIB_PKE_CCA_AMV_2017_H