// dpa framework - a collection of tools for differential power analysis
// Copyright (C) 2011  Garrett C. Smith
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>
#include <cstdio>
#include <cstring>
#include "aes.h"
#include "crypto.h"
#include "grostl.h"
#include "utility.h"

using namespace std;

namespace grostl {

// -----------------------------------------------------------------------------
// Perform the AddRoundConstant step for the P permutation
void add_round_const_p(const uint8_t *in, int round, uint8_t *out)
{
    for (int i = 0; i < 64; ++i) {
        if ((i & 7) == 0)
            out[i] = in[i] ^ ((i << 1) ^ round);
        else
            out[i] = in[i];
    }
}

// -----------------------------------------------------------------------------
// Perform the AddRoundConstant step for the Q permutation
void add_round_const_q(const uint8_t *in, int round, uint8_t *out)
{
    for (int i = 0; i < 64; ++i) {
        if ((i & 7) == 7)
            out[i] = in[i] ^ (((i & ~7) << 1) ^ 0xFF ^ round);
        else
            out[i] = in[i] ^ 0xFF;
    }
}

// -----------------------------------------------------------------------------
// Perform the SubBytes substitution step
void sub_bytes(const uint8_t *in, uint8_t *out)
{
    for (int i = 0; i < 64; ++i)
        out[i] = aes::sbox[in[i]];
}

// -----------------------------------------------------------------------------
// Perform the ShiftBytes step for the P permutation
void shift_bytes_p(const uint8_t *in, uint8_t *out)
{
    for (int i = 0; i < 64; ++i) {
        const int row = i & 7;
        const int col = ((i >> 3) + row) & 7;
        out[i] = in[(col << 3) | row];
    }
}

// -----------------------------------------------------------------------------
// Perform the ShiftBytes step for the Q permutation
void shift_bytes_q(const uint8_t *in, uint8_t *out)
{
    for (int i = 0; i < 64; ++i) {
        const int row = i & 7;
        const int col = ((i >> 3) + (row << 1) + (row < 4)) & 7;
        out[i] = in[(col << 3) | row];
    }
}

// -----------------------------------------------------------------------------
void mix_bytes(const uint8_t *in, uint8_t *out)
{
    for (int i = 0; i < 8; ++i, in += 8, out += 8) {
        out[0] = GF_2(in[0]) ^ GF_2(in[1]) ^ GF_3(in[2]) ^ GF_4(in[3]) ^
                 GF_5(in[4]) ^ GF_3(in[5]) ^ GF_5(in[6]) ^ GF_7(in[7]);
        out[1] = GF_7(in[0]) ^ GF_2(in[1]) ^ GF_2(in[2]) ^ GF_3(in[3]) ^
                 GF_4(in[4]) ^ GF_5(in[5]) ^ GF_3(in[6]) ^ GF_5(in[7]);
        out[2] = GF_5(in[0]) ^ GF_7(in[1]) ^ GF_2(in[2]) ^ GF_2(in[3]) ^
                 GF_3(in[4]) ^ GF_4(in[5]) ^ GF_5(in[6]) ^ GF_3(in[7]);
        out[3] = GF_3(in[0]) ^ GF_5(in[1]) ^ GF_7(in[2]) ^ GF_2(in[3]) ^
                 GF_2(in[4]) ^ GF_3(in[5]) ^ GF_4(in[6]) ^ GF_5(in[7]);
        out[4] = GF_5(in[0]) ^ GF_3(in[1]) ^ GF_5(in[2]) ^ GF_7(in[3]) ^
                 GF_2(in[4]) ^ GF_2(in[5]) ^ GF_3(in[6]) ^ GF_4(in[7]);
        out[5] = GF_4(in[0]) ^ GF_5(in[1]) ^ GF_3(in[2]) ^ GF_5(in[3]) ^
                 GF_7(in[4]) ^ GF_2(in[5]) ^ GF_2(in[6]) ^ GF_3(in[7]);
        out[6] = GF_3(in[0]) ^ GF_4(in[1]) ^ GF_5(in[2]) ^ GF_3(in[3]) ^
                 GF_5(in[4]) ^ GF_7(in[5]) ^ GF_2(in[6]) ^ GF_2(in[7]);
        out[7] = GF_2(in[0]) ^ GF_3(in[1]) ^ GF_4(in[2]) ^ GF_5(in[3]) ^
                 GF_3(in[4]) ^ GF_5(in[5]) ^ GF_7(in[6]) ^ GF_2(in[7]);
    }
}

// -----------------------------------------------------------------------------
void permute_p(const uint8_t *in, uint8_t *out)
{
    uint8_t temp[64];
    memcpy(out, in, 64);

    for (int round = 0; round < 10; ++round) {
        add_round_const_p(out, round, temp);
        sub_bytes(temp, out);
        shift_bytes_p(out, temp);
        mix_bytes(temp, out);
    }
}

// -----------------------------------------------------------------------------
void permute_q(const uint8_t *in, uint8_t *out)
{
    uint8_t temp[64];
    memcpy(out, in, 64);

    for (int round = 0; round < 10; ++round) {
        add_round_const_q(out, round, temp);
        sub_bytes(temp, out);
        shift_bytes_q(out, temp);
        mix_bytes(temp, out);
    }
}

// -----------------------------------------------------------------------------
void compress(const uint8_t *msg, const uint8_t *chain, uint8_t *out)
{
    printf("initial: %s\n", util::btoa(chain, 64).c_str());
    printf("block:   %s\n", util::btoa(msg, 64).c_str());

    uint8_t hm[64], p[64], q[64];
    for (int i = 0; i < 64; ++i)
        hm[i] = msg[i] ^ chain[i];

    permute_p(hm, p);
    permute_q(msg, q);

    printf("p_out:   %s\n", util::btoa(p, 64).c_str());
    printf("q_out:   %s\n", util::btoa(q, 64).c_str());

    for (int i = 0; i < 64; ++i)
        out[i] = chain[i] ^ p[i] ^ q[i];

    printf("f_out:   %s\n", util::btoa(out, 64).c_str());
}

// -----------------------------------------------------------------------------
void output_transform(const uint8_t *msg, uint8_t *out)
{
    uint8_t p[64];
    permute_p(msg, p);
    for (int i = 0; i < 32; ++i)
        out[i] = msg[i + 32] ^ p[i + 32];
}

// -----------------------------------------------------------------------------
void hash(const vector<uint8_t> &in, vector<uint8_t> &out)
{
    // TODO
    vector<uint8_t> state(64, 0), chain(64, 0), fout(64, 0);
    state.resize(64);
    state[0] = in[0];
    state[1] = in[1];
    state[2] = in[2];
    state[3] = 0x80;
    state[63] = 0x01;
    chain[62] = 0x01;

    out.resize(32);
    compress(&state[0], &chain[0], &fout[0]);
    output_transform(&fout[0], &out[0]);
}

};

