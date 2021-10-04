#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  //  Answer: At the last iteration, _pp_vstore_float may store values which are out of index 
  //  because it always deal with vector whose length is VECTOR_WIDTH
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_mask maskZeroExp, maskNonZeroExp, maskClamp, maskExp, maskFinal;
  __pp_vec_float result, v;
  __pp_vec_int e;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one  = _pp_vset_int(1);
  __pp_vec_float upperBound = _pp_vset_float(9.999999f);
  __pp_mask maskAll = _pp_init_ones();
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    maskClamp = _pp_init_ones(0);
    maskZeroExp = _pp_init_ones(0);
    maskExp = _pp_init_ones(0);

    _pp_vload_int(e, exponents + i, maskAll);
    _pp_vload_float(v, values + i, maskAll);
    _pp_vload_float(result, values + i, maskAll);

    //find the index of exponents whose value is equal to zero
    _pp_veq_int(maskZeroExp, zero, e, maskAll);

    //set 1 to those number with zero exponent term
    _pp_vset_float(result, 1.f, maskZeroExp);
    
    
    // Inverse maskIsNegative to generate "else" mask
    maskNonZeroExp = _pp_mask_not(maskZeroExp);
    
    if(N-i<VECTOR_WIDTH)
      maskFinal = _pp_init_ones(N-i);
    else
      maskFinal = _pp_init_ones();
 
    // avoid non-sense calculation
    maskNonZeroExp = _pp_mask_and(maskFinal,maskNonZeroExp);

    // pow
    _pp_vgt_int(maskExp, e, one, maskNonZeroExp);
    while(_pp_cntbits(maskExp) > 0){
      _pp_vmult_float(result, result, v, maskExp);
      _pp_vsub_int(e, e, one, maskExp);
      _pp_vgt_int(maskExp, e, one, maskNonZeroExp);
    }

    //find clamp mask
    _pp_vgt_float(maskClamp, result, upperBound, maskNonZeroExp);

    //set clamped value
    _pp_vset_float(result, 9.999999f, maskClamp);
  
    // Write results back to memory
    _pp_vstore_float(output + i, result, maskFinal);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
  }

  return 0.0;
}