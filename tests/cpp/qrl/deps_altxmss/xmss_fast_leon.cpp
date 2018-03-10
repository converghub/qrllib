// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <xmss-alt/algsxmss.h>
#include <xmssBasic.h>
#include "gtest/gtest.h"
#include <xmss-alt/algsxmss_fast.h>

namespace {
    TEST(DepsAltXmss, LeonsTest) {
        unsigned char h = 4;
        unsigned long long siglen = static_cast<unsigned long long int>(4 + 32 + 67 * 32 + h * 32);

        unsigned char pk[64];
        unsigned char sk[4+4*32];
        unsigned int n = 48;
        unsigned char seed[48];

        printf("before keygen");

        bds_state s;
        unsigned int k = 2;
        auto stack = std::vector<unsigned char>((h + 1)*n);
        auto stacklevels = std::vector<unsigned char>(h+1);
        auto auth = std::vector<unsigned char>((h)*n);
        auto keep = std::vector<unsigned char>((h >> 1)*n);
        auto treehash = std::vector<treehash_inst>(h - k);
        auto th_nodes = std::vector<unsigned char>((h-k)*n);
        auto retain = std::vector<unsigned char>(((1 << k) - k - 1)*n);
        unsigned int stackoffset = 0;
        bds_state *state = &s;

        for (unsigned i = 0; i < h-k; i++)
        {
            treehash[i].node = &th_nodes[n*i];
        }
        xmss_set_bds_state(state, stack.data(), stackoffset, stacklevels.data(),
          auth.data(), keep.data(), treehash.data(), retain.data(), 0);

        xmss_params params;
        xmss_set_params(&params, 32, h, 16, 2);

        xmssfast_Genkeypair(eHashFunction::SHAKE_128, &params, pk, sk, state, seed);

        unsigned char msg[32] = {0};
        unsigned char sign[10000];

        printf("Sign / Verify");

        int x;
        int y = xmssfast_update(eHashFunction::SHAKE_128, &params, sk, state, 10);

        x = xmssfast_Signmsg(eHashFunction::SHAKE_128, &params, sk, state, sign, msg,32);
        x = xmss_Verifysig(eHashFunction::SHAKE_128, &params.wots_par, msg,32, sign,pk, h);

        printf("\n%d\n",x);

        unsigned long long m = 32;
        x = xmssfast_Signmsg(eHashFunction::SHAKE_128, &params, sk, state, sign, msg,32);

        msg[10] ^= 1;
        x = xmss_Verifysig(eHashFunction::SHAKE_128, &params.wots_par, msg,32, sign,pk, h);

        printf("\n%d\n",x);

        msg[0]^=1;
        x = xmss_Verifysig(eHashFunction::SHAKE_128, &params.wots_par, msg,32, sign,pk, h);
        printf("\n%d\n",x);
        msg[0]^=1;
        sign[5*32]^=1;
        x = xmss_Verifysig(eHashFunction::SHAKE_128, &params.wots_par, msg,32, sign,pk, h);
        printf("\n%d\n",x);
    }
}
