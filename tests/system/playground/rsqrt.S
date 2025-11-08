.text
.globl  newton_step_asm
.align 2

newton_step_asm:
    addi sp, sp, -16
    sw   ra, 12(sp)
    sw   a0, 0(sp)        # save rec_inv_sqrt pointer
    sw   a1, 8(sp)        # save x

    # invsqrt = *rec_inv_sqrt;
    lhu  t0, 0(a0)        # t0 = invsqrt
    sw   t0, 4(sp)        # save invsqrt

    # invsqrt2 = ((uint32_t)invsqrt * (uint32_t)invsqrt) >> 16;
    mv   a0, t0
    mv   a1, t0
    jal  ra, mul_32       # a0 = invsqrt * invsqrt
    srl  t1, a0, 16       # t1 = invsqrt2

    # val = (3U << 16) - (uint32_t)x * (uint32_t)invsqrt2;
    li   t2, 3
    slli t2, t2, 16       # t2 = 3 << 16   (val)

    lw   a0, 8(sp)        # a0 = x
    mv   a1, t1           # a1 = invsqrt2
    jal  ra, mul_32       # a0 = x * invsqrt2
    sub  t2, t2, a0       # t2 = val

    # val >>= 2;
    srl  t2, t2, 2

    # val = val * (uint32_t)invsqrt >> 15;
    mv   a0, t2
    lw   a1, 4(sp)        # a1 = invsqrt
    jal  ra, mul_32       # a0 = val * invsqrt
    srl  t2, a0, 15       # t2 = val >> 15 (Q0.16)

    # *rec_inv_sqrt = (uint16_t)val;
    lw   t0, 0(sp)        # t0 = rec_inv_sqrt pointer
    sh   t2, 0(t0)        # store low 16 bits

    lw   ra, 12(sp)
    addi sp, sp, 16
    ret

mul_32:
    li   a3, 0           # result = 0
1:
    beq  a1, x0, 2f      # while (b != 0)
    andi a4, a1, 1
    beq  a4, x0, 3f
    add  a3, a3, a0      # result += a
3:
    slli a0, a0, 1       # a <<= 1
    srli a1, a1, 1       # b >>= 1
    j    1b
2:
    mv   a0, a3          # return result in a0
    ret
