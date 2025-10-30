# Performance counter functions for RISC-V
# Read 64-bit cycle counter

.text

.global uf8_decode
.align 2
uf8_decode:
	andi t2, a0, 0x0f        # mantissa = fl & 0x0f
	srli t3, a0, 4           # exponent = fl >> 4

	li   t6, 0x7FFF          # load constant 0x7FFF
	li   t5, 15              # prepare 15
	sub  t4, t5, t3          # t4 = 15 - exponent
	srl  t4, t6, t4          # t4 = 0x7FFF >> (15 - exponent)
	slli t4, t4, 4           # offset = t4 << 4

	sll  t2, t2, t3          # mantissa << exponent
	add  a0, t2, t4          # return = mantissa<<exponent + offset
	ret

.size uf8_decode,.-uf8_decode


############################
.global uf8_encode
uf8_encode:
        li t2, 16
        bgeu a0, t2, geu_16      # if (value < 16) return
        ret
geu_16:
        mv t4, a0                # set x as input
        li t2, 32                # n = 32
        li t3, 16                # c =16
clz: 
        srl t5, t4, t3           # y = x >> c
        beq t5, x0, temp_label   # if(y)
        sub t2, t2, t3           # n -= c
        mv t4, t5                # x = y
temp_label:
        srli t3, t3, 1
        bne t3, x0, clz
        
        sub t2, t2, t4           # lz = clz(value)
        li t3, 31
        sub t2, t3, t2           # msb = 31 - lz
        li t3, 0                 # exponent = 0
        li t4, 0                 # overflow = 0
        
        li t5, 5
        blt t2, t5, temp_label2  # if (msb >= 5)
        addi t3, t2, -4          # exponent = msb - 4;
        
        li t5, 15
        bltu t3, t5, temp_label3 # if (exponent > 15)
        li t3, 15                # exponent = 15
temp_label3:
        
        li t5, 0                 # e=0
start_loop:
        bge t5, t3, end_loop        # if e >= exponent, exit
        slli t6, t4, 1              # t6 = (overflow << 1)
        addi t4, t6, 16             # overflow = t6 + 16
        addi t5, t5, 1              # e++
        j start_loop
end_loop:
    
        bltu t3, x0, temp_label2 # if exponent <= 0, break while loop
        bgeu a0, t4, temp_label2 # if value >= overflow, break while loop
        addi t5, t4, -16         # t5 = (overflow - 16)
        srli t4, t4, 1           # overflow = t5 >> 1
        addi t3, t3, -1          # exponent--
        j end_loop               # back to the start of the while loop

temp_label2:  
        li t5, 15
        bgeu t3, t5, end_while   # if exponent >= 15, then break the while loop
        slli t5, t4, 1           # t5 = (overflow << 1)
        addi t5, t5, 16          # next_overflow t5 + 16
        blt a0, t5, end_while    # if (value < next_overflow) break
        mv t4, t5                # overflow = next_overflow;
        addi t3, t3, 1           # exponent++;
        j temp_label2
end_while:
        sub t5, a0, t4           # t5 = value - overflow
        srl t5 ,t5, t3           # mantissa = t5 >> exponent
        slli a0, t3, 4           # a0 = exponent << 4
        or a0, a0, t5
        ret

.size uf8_encode,.-uf8_encode
