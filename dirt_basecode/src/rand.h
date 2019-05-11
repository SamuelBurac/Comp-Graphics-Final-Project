/*
	Tiny self-contained version of the xoroshiro128+ Random Number Generator
    for C++ written by Wojciech Jarosz.

	The C++ class interface is modeled after the interface of Wenzel Jakob's
	PCG32 C++ wrapper, while the xoroshiro128+ generator itself is based on
	code from David Blackman and Sebastiano Vigna:

	----------------------------

	Written in 2016-2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

	To the extent possible under law, the author has dedicated all copyright
	and related and neighboring rights to this software to the public domain
	worldwide. This software is distributed without any warranty.

	See <http://creativecommons.org/publicdomain/zero/1.0/>.

	This is xoroshiro128+ 1.0, our best and fastest small-state generator
	for floating-point numbers. We suggest to use its upper bits for
	floating-point generation, as it is slightly faster than
	xoroshiro128**. It passes all tests we are aware of except for the four
	lower bits, which might fail linearity tests (and just those), so if
	low linear complexity is not considered an issue (as it is usually the
	case) it can be used to generate 64-bit outputs, too; moreover, this
	generator has a very mild Hamming-weight dependency making our test
	(http://prng.di.unimi.it/hwd.php) fail after 8 TB of output; we believe
	this slight bias cannot affect any application. If you are concerned,
	use xoroshiro128** or xoshiro256+.

	We suggest to use a sign test to extract a random Boolean value, and
	right shifts to extract subsets of bits.

	The state must be seeded so that it is not everywhere zero. If you have
	a 64-bit seed, we suggest to seed a splitmix64 generator and use its
	output to fill s.

	NOTE: the parameters (a=24, b=16, b=37) of this version give slightly
	better results in our test than the 2016 version (a=55, b=14, c=37).
*/

#pragma once

#include <stdint.h>

#define RNG_DEFAULT_STATE 0x853c49e6748fea9bULL

//! xoroshiro128+ pseudorandom number generator
struct RNG
{
	//! Initialize the pseudorandom number generator with the \ref seed() function
	RNG(uint64_t initstate = RNG_DEFAULT_STATE) { seed(initstate); }
	RNG(uint32_t xseed, uint32_t yseed) { seed(xseed, yseed); }

	//! Seed the pseudorandom number generator
	void seed(uint64_t initstate)
	{
		// Use splitmix64 to seed the state

		// This is a fixed-increment version of Java 8's SplittableRandom generator
		// See http://dx.doi.org/10.1145/2714064.2660195 and
		// http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html
		//
		// It is a very fast generator passing BigCrush, and it can be useful if
		// for some reason you absolutely want 64 bits of state; otherwise, we
		// rather suggest to use a xoroshiro128+ (for moderately parallel
		// computations) or xorshift1024* (for massively parallel computations)
		// generator.

		auto next64 = [&initstate]()
		{
			uint64_t z = (initstate += 0x9e3779b97f4a7c15);
			z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
			z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
			return z ^ (z >> 31);
		};

		uint64_t x = next64();
		state[0] = static_cast<uint32_t>(x >> 32);
		state[1] = static_cast<uint32_t>(x);
		x = next64();
		state[2] = static_cast<uint32_t>(x >> 32);
		state[3] = static_cast<uint32_t>(x);
	}

	//! Convenience function to see from 2 32-bit values (e.g. 2D integer coordinates)
	void seed(uint32_t xseed, uint32_t yseed)
	{
		// use x for high-order bits, and y for low-order bits
		seed((uint64_t(xseed) << 32) + uint32_t(yseed));
	}

	//! Generate a uniformly distributed unsigned 32-bit random number
	uint32_t nextUInt()
	{
		const uint32_t result_plus = state[0] + state[3];

		const uint32_t t = state[1] << 9;

		state[2] ^= state[0];
		state[3] ^= state[1];
		state[1] ^= state[2];
		state[0] ^= state[3];

		state[2] ^= t;

		state[3] = rotl(state[3], 11);

		return result_plus;
	}

	//! Generate a uniformly distributed number, r, where 0 <= r < bound
	uint32_t nextUInt(uint32_t bound)
	{
		// To avoid bias, we need to make the range of the RNG a multiple of
		// bound, which we do by dropping output less than a threshold.
		// A naive scheme to calculate the threshold would be to do
		//
		//     uint32_t threshold = 0x100000000ull % bound;
		//
		// but 64-bit div/mod is slower than 32-bit div/mod (especially on
		// 32-bit platforms).  In essence, we do
		//
		//     uint32_t threshold = (0x100000000ull-bound) % bound;
		//
		// because this version will calculate the same modulus, but the LHS
		// value is less than 2^32.

		uint32_t threshold = (~bound + 1u) % bound;

		// Uniformity guarantees that this loop will terminate.  In practice, it
		// should usually terminate quickly; on average (assuming all bounds are
		// equally likely), 82.25% of the time, we can expect it to require just
		// one iteration.  In the worst case, someone passes a bound of 2^31 + 1
		// (i.e., 2147483649), which invalidates almost 50% of the range.  In
		// practice, bounds are typically small and only a tiny amount of the
		// range is eliminated.
		for (;;)
		{
			uint32_t r = nextUInt();
			if (r >= threshold)
				return r % bound;
		}
	}

	//! Generate a single precision floating point value on the interval [0, 1)
	float nextFloat()
	{
		/* Trick from MTGP: generate an uniformly distributed
		   single precision number in [1,2) and subtract 1. */
		union
		{
			uint32_t u;
			float f;
		} x;
		x.u = (nextUInt() >> 9) | 0x3f800000u;
		return x.f - 1.0f;
	}

	//! Generate a double precision floating point value on the interval [0, 1)
	/*!
		\remark Since the underlying random number generator produces 32 bit
		output, only the first 32 mantissa bits will be filled (however, the
		resolution is still finer than in \ref nextFloat(), which only uses 23
		mantissa bits).
	 */
	double nextDouble()
	{
		/* Trick from MTGP: generate an uniformly distributed
		   double precision number in [1,2) and subtract 1. */
		union
		{
			uint64_t u;
			double d;
		} x;
		x.u = ((uint64_t) nextUInt() << 20) | 0x3ff0000000000000ULL;
		return x.d - 1.0;
	}

	//! Equality operator
	bool operator==(const RNG &other) const
	{
		return state[0] == other.state[0] &&
			   state[1] == other.state[1] &&
			   state[2] == other.state[2] &&
			   state[3] == other.state[3];
	}

	//! Inequality operator
	bool operator!=(const RNG &other) const { return !(*this == other); }

	uint32_t state[4];  // RNG state

private:

	static uint32_t rotl(const uint32_t x, int k)
	{
		return (x << k) | (x >> (32 - k));
	}
};

inline float randf()
{
	static RNG globalRNG = RNG();
	return globalRNG.nextFloat();
}


inline Vector2f randomInUnitDisk()
{
	Vector2f p;
	do
	{
        float a = randf();
        float b = randf();
		p = 2.0f * Vector2f(a, b) - Vector2f::Ones();
	} while (p.squaredNorm() >= 1.f);

	return p;
}


inline Vector3f randomInUnitSphere()
{
	Vector3f p;
	do
	{
        float a = randf();
        float b = randf();
        float c = randf();
		p = 2.0f * Vector3f(a, b, c) - Vector3f::Ones();
	} while (p.squaredNorm() >= 1.0f);

	return p;
}
