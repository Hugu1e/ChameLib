#ifndef CH_FS_ECC_CCT_2024_H
#define CH_FS_ECC_CCT_2024_H

#include <stdio.h>
#include <base/PbcElements_copy.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

enum{g};
enum{y};
enum{x};
enum{z1, z2, c1};


class CH_FS_ECC_CCT_2024_pp: public PbcElements_copy{};
class CH_FS_ECC_CCT_2024_pk: public PbcElements_copy{};
class CH_FS_ECC_CCT_2024_sk: public PbcElements_copy{};
class CH_FS_ECC_CCT_2024_r: public PbcElements_copy{};

class CH_FS_ECC_CCT_2024: public PbcScheme{
    private:
        element_t rho;
        element_t t1,t2,T1,T2,c2;

    public:
        CH_FS_ECC_CCT_2024(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void SetUp(CH_FS_ECC_CCT_2024_pp &pp, CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_r &r_p);
        
        void KeyGen(CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, CH_FS_ECC_CCT_2024_pp &pp);

        void H(element_t res, element_t m);
        void H(element_t res, element_t m1, element_t m2, element_t m3, element_t m4);

        void Hash(element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pk &pk, element_t m, CH_FS_ECC_CCT_2024_pp &pp);

        bool Check(CH_FS_ECC_CCT_2024_pk &pk, element_t m, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp);
        
        void Adapt(CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pk &pk, CH_FS_ECC_CCT_2024_sk &sk, element_t m, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r, CH_FS_ECC_CCT_2024_pp &pp);

        bool Verify(CH_FS_ECC_CCT_2024_pk &pk, element_t m_p, element_t h, CH_FS_ECC_CCT_2024_r &r_p, CH_FS_ECC_CCT_2024_pp &pp);

        ~CH_FS_ECC_CCT_2024();
};

#endif  //CH_FS_ECC_CCT_2024_H