###### tags: `PP`

# Parallel Programming HW1

## Q1

### Q1-1
After sweeping the value of `VECTOR_WIDTH` defined in `def.h`, do the following command to see the change of vector utilization:
```
./myexp -s 10000
```
Vector utilization when sweeping `VECTOR_WIDTH`
|  2   |  4   |  8   | 16  |
|:----:|:----:|:----:|:---:|
| 83.6% | 78.3% | 75.6% | 74.4%    |

Notice that the vector utilization **decreases** with increase of `VECTOR_WIDTH`. Most of time the incative vector lane happens when we are doing the exponent calculation, which is combined with a series of multiplications. In my implementation, I use a `mask` to record which number in the vector should keep multiplying itself to reach the target exponent. During the calculation in the `while loop`,  numbers in the vector stop calculation successively, and **there would be more and more `0` in the mask at each iteration**. 
Consider an extreme case with`VECTOR_WIDTH=2`, if there is only one number needs to be calculated while the others are done, then we should do `n` iterations until the number reach its target exponent. The mask for the other index should always be `0` during the iteration. So it actually causes `n*1=n` incative vector lane. The total vector utilization during the exponent calculation should be `n/2n = 0.5`. Here comes another extreme case with `VECTOR_WIDTH=16`, if the same thing happens(only one `1`, `1`*1 and `0`*15) like the previous example, the total vector utilization during the exponent calculation should be `n/16n = 0.0625`. 
As we can see, **when the `VECTOR_WIDTH` increases**, **it costs more numbers in the vector to wait for the exetreme case**(number with large exponent). In other words, the inactive vector lane would increase, so the vector utilization decreases certainly.

## Q2
### Q2-1
**AVX2**  expands  most integer commands from 128(**16 bytes**) to 256 bits(**32 bytes**), so `__builtin_assume_aligned(a, 16);` doesn't align arrays propely. As a result, all we need to do is to change the number of shifted bytes as the following:

```cpp=
a = (float *)__builtin_assume_aligned(a, 32);
b = (float *)__builtin_assume_aligned(b, 32);
c = (float *)__builtin_assume_aligned(c, 32);
```

Then we see the result can by a command `diff assembly/test1.vec.restr.align.avx2.s assembly/not_aligned.s` :
```shell=
50,53c50,53
<       vmovaps (%rbx,%rcx,4), %ymm0
<       vmovaps 32(%rbx,%rcx,4), %ymm1
<       vmovaps 64(%rbx,%rcx,4), %ymm2
<       vmovaps 96(%rbx,%rcx,4), %ymm3
---
>       vmovups (%rbx,%rcx,4), %ymm0
>       vmovups 32(%rbx,%rcx,4), %ymm1
>       vmovups 64(%rbx,%rcx,4), %ymm2
>       vmovups 96(%rbx,%rcx,4), %ymm3
58,61c58,61
<       vmovaps %ymm0, (%r14,%rcx,4)
<       vmovaps %ymm1, 32(%r14,%rcx,4)
<       vmovaps %ymm2, 64(%r14,%rcx,4)
<       vmovaps %ymm3, 96(%r14,%rcx,4)
---
>       vmovups %ymm0, (%r14,%rcx,4)
>       vmovups %ymm1, 32(%r14,%rcx,4)
>       vmovups %ymm2, 64(%r14,%rcx,4)
>       vmovups %ymm3, 96(%r14,%rcx,4)
```

### Q2-2

Runtime:

| case 1 | case 2 | case 3 |
| -------- | -------- | -------- |
| 8.32s     | 2.77s     | 1.52s     |

Speedup(set the original case as baseline):

| original | vectorized | AVX2 |
| -------- | ---------- | ---- |
| 1x       | 3x         | 5x   |

The size of `float` is **32 bits**, and we achive roughly **3x** speedup after vectorizing the code, so the bit width of the default vector registers on the PP machines should be **128 bits**. Secondly, AVX2 achive nearly **2x** faster than the vectorized one, so the bit width of AVX2 vector registers should be **256 bits**.

### Q2-3

Because the original code has unsafe dependent memory operation(**write-after-write, WAW**), the compiler is unable to generate vectorized assembly code. After we declare the complete if-else branch, there is no **WAW** operation, so the compiler can generate a vectorized code. That is what makes these two different results.
