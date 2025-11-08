#include <stdint.h>
#include "print.h"

extern uint32_t newton_step_asm(uint16_t *rec_inv_sqrt, uint16_t x);

#define REC_INV_SQRT_CACHE (16)
static const uint16_t inv_sqrt_cache[REC_INV_SQRT_CACHE] = {
    65535, 65535, 46340, 37837,
    32768, 29308, 26754, 24770,
    23170, 21845, 20724, 19759,
    18918, 18176, 17515, 16921
};

static const uint16_t inv_sqrt_ans_cache[REC_INV_SQRT_CACHE] = {
	65535, 46340, 32768, 23170,
	16384, 11585, 8192, 5792,
	4096, 2896, 2048, 1448,
	1024, 724, 512, 362
};

static void newton_step(uint16_t *rec_inv_sqrt, uint16_t x) {
    uint16_t invsqrt, invsqrt2;
    uint32_t val;

    invsqrt = *rec_inv_sqrt;
    invsqrt2 = ((uint32_t)invsqrt * (uint32_t)invsqrt) >> 16;
    val = (3LL << 16) - (uint32_t)x * (uint32_t)invsqrt2;
    val >>= 2;
    val = val* (uint32_t)invsqrt >> 15;
    *rec_inv_sqrt = (uint16_t)val;
}

uint16_t rec_inv_sqrt(uint16_t x) {
    if (x == 0) {
        return 65535; // or some large sentinel
    }

    uint16_t orig_x = x;
    uint32_t shift = 0;

    // 1) scale x down so it falls into [1, 15]
    while (x > REC_INV_SQRT_CACHE - 1) {
        // divide by 4 with rounding
        x = (x + 2) >> 2;
        ++shift;
    }
    if (x == 0) {
        x = 1;
    }
    
    // now 1 <= x <= 15, treat it as "n"
    uint16_t n = x;

    // 2) get initial guess for 1/sqrt(n) from LUT (Q0.16)
    //uint16_t y = (1UL << 15); 
    uint16_t y = inv_sqrt_cache[n];
    
    // 3) scale guess from 1/sqrt(n) to 1/sqrt(orig_x)
    //    because orig_x ≈ n * 4^shift => 1/sqrt(orig_x) = 1/sqrt(n)/2^shift
    y >>= shift;

    // 4) a couple of Newton iterations using the ORIGINAL x
    //    (orig_x is the actual value whose inverse sqrt we want)
    newton_step_asm(&y, orig_x);
    newton_step_asm(&y, orig_x);
    newton_step_asm(&y, orig_x);
    
    return y;
}

static void print_five_digits_dec(int32_t val)
{
    char buf[5];

    // Null-terminate for safety (not strictly needed for printstr)
    buf[4] = '\0';

    // Fill from least significant digit to most, right to left
    for (int i = 3; i >= 0; i--) {
        buf[i] = '0' + umod(val, 10);
        val = udiv(val, 10);
    }

    // Always print exactly 5 characters, including leading zeros
    printstr(buf, 5);
}

// 16-bit fixed: value = q / 2^16
static void print_q0_16(uint16_t q) {
    // 4 decimal digits: floor(q * 10000 / 2^16)
    uint32_t scaled = (uint32_t)q * 10000U;  // still 32-bit safe here
    uint32_t frac   = udiv(scaled, (1UL << 16));

    TEST_LOGGER("0.");
    print_five_digits_dec((unsigned long)frac);
}



void run_rsqrt_test() {
    for (int i = 0; i < 16; i++) {
        uint16_t arg = 1U << i;
        uint16_t result = rec_inv_sqrt(arg);  // Q0.16 ~ 1/sqrt(arg)
		uint16_t answer = inv_sqrt_ans_cache[i];
		
        // Absolute difference
        int32_t diff = (int32_t)answer - (int32_t)result;
        if (diff < 0) diff = -diff;

        // Optional: relative error * 10000 (only integers)
        uint32_t rel_err_x10000 = udiv((uint32_t)diff * 10000U, (int32_t)answer);

        TEST_LOGGER("input: ");
        print_dec((unsigned long)arg);

        TEST_LOGGER(" result≈");
        print_q0_16(result);

        TEST_LOGGER("  correct answer=");
        print_q0_16(answer);

        TEST_LOGGER("  rel_err=0.");
        print_five_digits_dec((unsigned long)rel_err_x10000);
        TEST_LOGGER("\n");
    }
}

