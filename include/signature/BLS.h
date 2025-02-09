#ifndef CHAMELIB_BLS_H
#define CHAMELIB_BLS_H
#include <base/PbcElements_copy.h>
#include <base/PbcScheme.h>
#include <utils/Hash.h>

class BLS_pp: public PbcElements_copy{};
class BLS_pk: public PbcElements_copy{};
class BLS_sk: public PbcElements_copy{};
class BLS_signature: public PbcElements_copy{};

class BLS: public PbcScheme{
    private:
        void H(element_t res, std::string m);

    public:
        BLS(element_s *_G1, element_s *_G2, element_s *_GT, element_s *_Zn);

        void Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature);
        void Setup(BLS_pp &pp, BLS_pk &pk, BLS_sk &sk, BLS_signature &signature, element_t _g);

        void KeyGen(BLS_pk &pk, BLS_sk &sk, BLS_pp &pp);

        void Sign(BLS_signature &signature, BLS_sk &sk, std::string message);

        bool Verify(BLS_pp &pp, BLS_pk &pk, std::string message, BLS_signature &signature);

        ~BLS() override;
        
        enum {
            g
        };

        enum {
            a
        };

        enum {
            y
        };

        enum {
            sigma
        };
};

#endif  // CHAMELIB_BLS_H