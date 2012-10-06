#ifndef __BASE_IEEE_FLOAT_H
#define __BASE_IEEE_FLOAT_H

namespace Yttrium
{

template <typename T>
class IeeeFloatBase
{
};

template <>
class IeeeFloatBase<float>
{
protected:

	typedef int_fast32_t FastSigned;
	typedef uint_fast32_t FastUnsigned;
	typedef int32_t Signed;
	typedef uint32_t Unsigned;

	static const FastSigned MantissaBits = 23;
	static const FastSigned ExponentBits = 8;
};

template <typename T>
class IeeeFloat: public IeeeFloatBase<T>
{
public:

	typedef typename IeeeFloatBase<T>::FastSigned FastSigned;
	typedef typename IeeeFloatBase<T>::FastUnsigned FastUnsigned;

	typedef typename IeeeFloatBase<T>::Signed Signed;
	typedef typename IeeeFloatBase<T>::Unsigned Unsigned;

	using IeeeFloatBase<T>::MantissaBits;
	using IeeeFloatBase<T>::ExponentBits;

public:

	union Union
	{
		T f;
		Unsigned i;
	};

public:

	static const FastUnsigned SignMask = 1 << (ExponentBits + MantissaBits);
	static const FastUnsigned ExponentMask = ((1 << ExponentBits) - 1) << MantissaBits;

	static const FastSigned ImplicitOne = 1 << MantissaBits;
	static const FastUnsigned MantissaMask = ImplicitOne - 1;

	static const FastSigned ExponentBias = (1 << (ExponentBits - 1)) - 1;
	static const FastSigned DenormalizedExponent = 1 - ExponentBias;
	static const FastSigned BiasedInfinityExponent = (1 << ExponentBits) - 1;
	static const FastSigned ImplicitMantissaMask = ImplicitOne | MantissaMask;
};

// To iterate over all finite positive floats, run an Unsigned counter from 1 to ExponentMask.

} // namespace Yttrium

#endif // __BASE_IEEE_FLOAT_H
