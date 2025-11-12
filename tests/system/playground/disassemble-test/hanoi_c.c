#include "print.h"


void hanoi_c(int n, char *from_pillar, char *to_pillar, char *aux_pillar){
    if(n==1){
    	TEST_LOGGER("Move disk 1 from ");
    	printstr(from_pillar, 1);
    	TEST_LOGGER(" to ");
    	printstr(to_pillar, 1);
    	TEST_LOGGER("\n");
        return;
    }
    hanoi_c(n-1, from_pillar, aux_pillar, to_pillar);
    TEST_LOGGER("Move disk ");
	print_dec((unsigned long) n);
	TEST_LOGGER(" from ");
	printstr(from_pillar, 1);
	TEST_LOGGER(" to ");
	printstr(to_pillar, 1);
	TEST_LOGGER("\n");
    hanoi_c(n-1, aux_pillar, to_pillar, from_pillar);
}
