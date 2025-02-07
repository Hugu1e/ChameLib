#ifndef CH_AMV_2017_H
#define CH_AMV_2017_H

#include <base/PbcScheme.h>
#include <base/PbcElements.h>
#include <utils/Hash.h>
#include <utils/Logger.h>
#include <AE/PKE_CCA_AMV_2017.h>
#include <AE/PKE_CPA_AMV_2017.h>

class CH_AMV_2017_pk{
    private:
        PbcElements CH_pk;
        PKE_CCA_AMV_2017_pk PKE_CCA_pk;
        PKE_CPA_AMV_2017_pk PKE_CPA_pk;
    public:
        PbcElements* get_CH_pk(){
            return &CH_pk;
        }
        PKE_CCA_AMV_2017_pk* get_PKE_CCA_pk(){
            return &PKE_CCA_pk;
        }
        PKE_CPA_AMV_2017_pk* get_PKE_CPA_pk(){
            return &PKE_CPA_pk;
        }
};

class CH_AMV_2017_sk{
    private:
        PbcElements CH_sk;
        PKE_CCA_AMV_2017_sk PKE_CCA_sk;
        PKE_CPA_AMV_2017_sk PKE_CPA_sk;
    public:
        PbcElements* get_CH_sk(){
            return &CH_sk;
        }
        PKE_CCA_AMV_2017_sk* get_PKE_CCA_sk(){
            return &PKE_CCA_sk;
        }
        PKE_CPA_AMV_2017_sk* get_PKE_CPA_sk(){
            return &PKE_CPA_sk;
        }
};

class CH_AMV_2017_r{
    private:
        PKE_CPA_AMV_2017_c c1;
        PKE_CCA_AMV_2017_c c2;
        PbcElements pai;
    public:
        PKE_CPA_AMV_2017_c* get_c1(){
            return &c1;
        }
        PKE_CCA_AMV_2017_c* get_c2(){
            return &c2;
        }
        PbcElements* get_pai(){
            return &pai;
        }
};

class CH_AMV_2017_h{
    private:
        PbcElements h;
        CH_AMV_2017_r r;
    public:
        PbcElements *get_h(){
            return &h;
        }
        CH_AMV_2017_r *get_r(){
            return &r;
        }
};

class CH_AMV_2017: public PbcScheme{
    private:
        PKE_CCA_AMV_2017 PKE_CCA;
        PKE_CPA_AMV_2017 PKE_CPA;
        void H(element_t res, element_t m1, element_t m2, element_t n);

    public:
        CH_AMV_2017(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void KeyGen(CH_AMV_2017_pk *pk, CH_AMV_2017_sk *sk);

        void Hash(CH_AMV_2017_h *h, element_t m, CH_AMV_2017_pk *pk);

        bool Check(CH_AMV_2017_h *h, element_t m, CH_AMV_2017_pk *pk);

        void Adapt(CH_AMV_2017_h *h_p, element_t m_p, CH_AMV_2017_h *h, element_t m, CH_AMV_2017_sk *sk, CH_AMV_2017_pk *pk);

        bool Verify(CH_AMV_2017_h *h_p, element_t m_p, CH_AMV_2017_pk *pk);

        ~CH_AMV_2017();
};

#endif  //CH_AMV_2017_H