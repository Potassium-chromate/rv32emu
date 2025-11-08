#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "print.h"


typedef uint8_t uf8;
extern uint64_t get_cycles(void);
extern uint64_t get_instret(void);
extern void hanoi(void);
extern void hanoi_c(int n, char *from_pillar, char *to_pillar, char *aux_pillar);
extern void hanoi_improve(int n, char from_pillar, char to_pillar, char aux_pillar);
extern uf8 uf8_encode(uint32_t value);
extern uint32_t uf8_decode(uf8 fl);
extern void run_rsqrt_test();

static bool test(void)
{
    int32_t previous_value = -1;
    bool passed = true;

    for (int i = 0; i < 256; i++) {
        uint8_t fl = i;
        int32_t value = uf8_decode(fl);
        uint8_t fl2 = uf8_encode(value);

        if (fl != fl2) {
            print_hex(fl);
            TEST_LOGGER(": produces value ");
            print_dec(value);
            TEST_LOGGER(" but encodes back to ");
            print_hex(fl2);
            TEST_LOGGER("\n");
            passed = false;
        }

        if (value <= previous_value) {
            print_hex(fl);
            TEST_LOGGER(" value ");
            print_dec(value);
            TEST_LOGGER(" <= previous_value ");
            print_dec(previous_value);
            TEST_LOGGER("\n");
            passed = false;
        }

        previous_value = value;
    }

    return passed;
}


int main(void)
{
    uint64_t start_cycles, end_cycles, cycles_elapsed;
    uint64_t start_instret, end_instret, instret_elapsed;

    TEST_LOGGER("\n=== Tests ===\n\n");

//    if (test()) {
//        TEST_LOGGER("All tests passed.\n");
//    }
    
    start_cycles = get_cycles();
    start_instret = get_instret();
    
    //hanoi();
    //hanoi_c(3, "A", "C", "B");
    //hanoi_improve(3, 'A', 'C', 'B');
    run_rsqrt_test();
    
    end_cycles = get_cycles();
    end_instret = get_instret();
    cycles_elapsed = end_cycles - start_cycles;
    instret_elapsed = end_instret - start_instret;
    
    TEST_LOGGER("  Cycles: ");
    print_dec((unsigned long) cycles_elapsed);
    TEST_LOGGER("\n");
    TEST_LOGGER("  Instructions: ");
    print_dec((unsigned long) instret_elapsed);
    TEST_LOGGER("\n");
    
    TEST_LOGGER("\n=== All Tests Completed ===\n");

    return 0;
}
