#include <stdio.h>
/*
* 	Fast Inverse Square Root algorithm, first appeared in Quaqe III Arena, was once the fastest way to compute 1/sqrt(y),
*	but not anymore (check SSE instruction rsqrtss). See explanation bellow.
*/
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
 	y  = number;
 	i  = * ( long * ) &y;                       // evil floating point bit level hacking
 	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
 	y  = * ( float * ) &i;
 	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
 // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

 	return y;
}
/*
*	31	30		   23	22				0
*	=	=============	=================	--> float 32 bits representation
*	Sign	 Exponent			 Mantissa		2^23 * E + M (binary) => (1 + M/2^23) * 2^(E-127) (decimal)
*	
*	E in [128, - 127]
*
*	log(1 + x) ~ x + μ, x - float in [0, 1]
*	μ = 0.0430, smallest average error
*	
*	=> log( (1 + M/2^23) * 2^(E-127) ) = log( 1 + M/2^23 ) + E - 127 = M/2^23 + E + μ - 127 = 1/2^23 (2^23 * E + M) + μ -127
*	=> y (binary) ~ log(y)
*
*	Float type is not made for bit manipulation, therefore, it has to be converted to int, but since i = ( long ) y; changes float to integer,
*	thus changing the bit order, i = * ( long * ) &y; is used to change the memory address from float to int and keep the bits in the same order.
*	
*	The goal is to calculate 1/sqrt(y) and since log(y) ~ i is enough to calculate log( 1/sqrt(y) ), because:
*
*	log( 1/sqrt(y) ) = log( y^(-2) ) = -1/2 * log(y) ~ -(i >> 1).		( << <=> *= 2 and >> <=> /=2 )
*	
*	If Γ is the solution of Γ = 1/sqrt(y)
*	=> log(Γ) = -1/2 * log(y) => 1/2^23 (2^23 * EΓ + MΓ) + μ -127 = -1/2 * (1/2^23 (2^23 * Ey + My) + μ -127) => ... =>
*	=> (2^23 * EΓ + MΓ) = 3/2 * 2^23 * (127 - μ) - 1/2 * (2^23 * Ey + My) => Γ = 0x5f3759df - ( i >> 1).
*
*	After an approximative value for 1/sqrt(y) was found it is refined by using a single iteration of Newton's method.
*/
