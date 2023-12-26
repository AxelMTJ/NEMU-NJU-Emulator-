#include "cpu/cpu.h"
#define DEBUG
#ifdef DEBUG
#define trace_alu(format, args...)                         \
	do                                                     \
	{                                                      \
		printf("[ALU:%d] " format "\n", __LINE__, ##args); \
	} while (0)
#else
#define trace_alu(format, args...) ((void)0)
#endif

uint32_t cutData(uint32_t data, uint32_t datasize)
{
	data <<= datasize;
	data >>= datasize;
	return data;
}

uint32_t setPF(uint32_t data)
{
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (data & (1 << i))
		{
			count++;
		}
	}
	return (count % 2) ? 0 : 1;
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t shiftbits = 32 - data_size;
	src <<= shiftbits;
	src >>= shiftbits;
	dest <<= shiftbits;
	dest >>= shiftbits;
	uint32_t result = src + dest;
	result <<= shiftbits;
	result >>= shiftbits;
	cpu.eflags.SF = ((result >> (data_size - 1)) == 1) ? 1 : 0;
	uint32_t srcSign = src >> (data_size - 1), destSign = dest >> (data_size - 1), resSign = result >> (data_size - 1);
	cpu.eflags.OF = ((srcSign && destSign && !resSign) || (!srcSign && !destSign && resSign)) ? 1 : 0;
	// Calculate the carry by checking if the result is less than either source or destination.
	if (result < src || result < dest)
	{
		cpu.eflags.CF = 1;
	}
	else
	{
		cpu.eflags.CF = 0;
	}

	// Calculate the parity (PF) directly based on the result.
	// PF is set if the number of set bits in the least significant byte is even.
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (result & (1 << i))
		{
			count++;
		}
	}
	cpu.eflags.PF = (count % 2 == 0) ? 1 : 0;

	// Set the Zero Flag (ZF) based on the least significant data_size bits.

	cpu.eflags.ZF = (result == 0) ? 1 : 0;

	return result;
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	//trace_alu("src %x, dest %x, size %d, CF %d", src, dest, data_size, cpu.eflags.CF);
	uint32_t shiftbits = 32 - data_size;
	src <<= shiftbits;
	src >>= shiftbits;
	dest <<= shiftbits;
	dest >>= shiftbits;
	uint32_t result = src + dest + cpu.eflags.CF, resultWithoutCF = src + dest;
	result <<= shiftbits;
	result >>= shiftbits;
	resultWithoutCF <<= shiftbits;
	resultWithoutCF >>= shiftbits;
	cpu.eflags.SF = ((result >> (data_size - 1)) == 1) ? 1 : 0;
	uint32_t srcSign = src >> (data_size - 1), destSign = dest >> (data_size - 1), resSign = result >> (data_size - 1);
	cpu.eflags.OF = ((srcSign && destSign && !resSign) || (!srcSign && !destSign && resSign)) ? 1 : 0;
	// Calculate the carry by checking if the result is less than either source or destination.
	if (result < src || result < dest || resultWithoutCF < src || resultWithoutCF < dest)
	{
		cpu.eflags.CF = 1;
	}
	else
	{
		cpu.eflags.CF = 0;
	}
	// Calculate the parity (PF) directly based on the result.
	// PF is set if the number of set bits in the least significant byte is even.
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (result & (1 << i))
		{
			count++;
		}
	}
	cpu.eflags.PF = (count % 2 == 0) ? 1 : 0;
	// Set the Zero Flag (ZF) based on the least significant data_size bits.
	cpu.eflags.ZF = (result == 0) ? 1 : 0;
	return result;
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t shiftbits = 32 - data_size;
	src <<= shiftbits;
	src >>= shiftbits;
	dest <<= shiftbits;
	dest >>= shiftbits;
	uint32_t result = dest - src;
	result <<= shiftbits;
	result >>= shiftbits;
	if (result > dest || src > dest)
		cpu.eflags.CF = 1;
	else
		cpu.eflags.CF = 0;
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (result & (1 << i))
		{
			count++;
		}
	}
	cpu.eflags.PF = (count % 2 == 0) ? 1 : 0;
	cpu.eflags.ZF = (result == 0) ? 1 : 0;
	cpu.eflags.SF = ((result >> (data_size - 1)) == 1) ? 1 : 0;
	uint32_t srcSign = src >> (data_size - 1), destSign = dest >> (data_size - 1), resSign = result >> (data_size - 1);
	cpu.eflags.OF = ((srcSign && !destSign && resSign) || (!srcSign && destSign && !resSign)) ? 1 : 0;

	return result;

#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	//trace_alu("src %x, dest %x, size %d, CF %d", src, dest, data_size, cpu.eflags.CF);
	uint32_t shiftbits = 32 - data_size;
	src <<= shiftbits;
	src >>= shiftbits;
	dest <<= shiftbits;
	dest >>= shiftbits;
	uint32_t result = dest - src - cpu.eflags.CF, resWithoutCF = dest - src;
	result <<= shiftbits;
	result >>= shiftbits;
	resWithoutCF <<= shiftbits;
	resWithoutCF >>= shiftbits;
	if (result > dest || src > dest || resWithoutCF < result)
		cpu.eflags.CF = 1;
	else
		cpu.eflags.CF = 0;
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (result & (1 << i))
		{
			count++;
		}
	}
	cpu.eflags.PF = (count % 2 == 0) ? 1 : 0;
	cpu.eflags.ZF = (result == 0) ? 1 : 0;
	cpu.eflags.SF = ((result >> (data_size - 1)) == 1) ? 1 : 0;
	uint32_t srcSign = src >> (data_size - 1), destSign = dest >> (data_size - 1), resSign = result >> (data_size - 1);
	cpu.eflags.OF = ((srcSign && !destSign && resSign) || (!srcSign && destSign && !resSign)) ? 1 : 0;
	return result;
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	uint32_t mask = 0xffffffff >> (32 - data_size);
	src &= mask;
	dest &= mask;
	uint64_t res = (uint64_t)src * dest;
	//res=cutData(res,32-data_size);
	//trace_alu("src %x, dest %x, size %d, res %x", src, dest, data_size, res);
	uint64_t mask2 = 0xffffffffffffffff << data_size;
	uint64_t CFdet = res & mask2;
	if (CFdet != 0)
	{
		cpu.eflags.CF = 1;
		cpu.eflags.OF = 1;
	}
	else
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
	}

	return res;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else

	//uint32_t destSign=dest>>(data_size-1);
	int64_t res;
	if (data_size == 8)
		res = (int8_t)src * dest;
	else
	{
		if (data_size == 16)
			res = (int16_t)src * dest;
		else
			res = (int64_t)src * dest;
	}

	//res=cutData(res,32-data_size);
	//trace_alu("src %x, dest %x, size %d, res %x", src, dest, data_size, res);

	return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	if (src == 0)
		return 0;
	uint64_t mask = 0xffffffffffffffff;
	mask >>= (64 - data_size);
	src &= mask;
	dest &= mask;
	uint32_t res = dest / src;
	return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	if(src==0)
		return 0;
	int64_t res;
	if (data_size == 8)
		res = (int8_t)dest/src;
	else
	{
		if (data_size == 16)
			res = (int16_t)dest/src;
		else
			res = (int64_t)dest/src;
	}
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	if (src == 0)
		return 0;
	uint32_t res = dest % src;
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	if (src == 0)
		return 0;
	uint32_t res = dest % src;
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = src & dest;
	res = cutData(res, 32 - data_size);
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	cpu.eflags.SF = ((res >> (data_size - 1)) == 1) ? 1 : 0;
	return res;
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = src ^ dest;
	res = cutData(res, 32 - data_size);
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	cpu.eflags.SF = ((res >> (data_size - 1)) == 1) ? 1 : 0;
	return res;
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = src | dest;
	res = cutData(res, 32 - data_size);
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	cpu.eflags.SF = ((res >> (data_size - 1)) == 1) ? 1 : 0;
	return res;
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = dest << src, resDetCF = dest << (src - 1);
	res = cutData(res, 32 - data_size);
	resDetCF = cutData(resDetCF, 32 - data_size);
	cpu.eflags.CF = (resDetCF >> (data_size - 1));
	cpu.eflags.SF = (res >> (data_size - 1));
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	return res;
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = dest >> src, resDetCF = dest >> (src - 1);
	res = cutData(res, 32 - data_size);
	resDetCF = cutData(resDetCF, 32 - data_size);
	cpu.eflags.CF = (resDetCF & 0x1);
	cpu.eflags.SF = (res >> (data_size - 1));
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	return res;
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t destSign = dest >> (data_size - 1), res = dest >> src, resDetCF = dest >> (src - 1);
	if (destSign)
	{
		uint32_t mask = 0xffffffff;
		mask <<= (data_size - src - 1);
		res |= mask;
	}
	res = cutData(res, 32 - data_size);

	resDetCF = cutData(resDetCF, 32 - data_size);
	cpu.eflags.CF = (resDetCF & 0x1);
	cpu.eflags.SF = (res >> (data_size - 1));
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	// trace_alu("src %x, dest %x, size %d, res %x, sign %x", src, dest, data_size, res,destSign);
	return res;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	src = cutData(src, 32 - data_size);
	dest = cutData(dest, 32 - data_size);
	uint32_t res = dest << src, resDetCF = dest << (src - 1);
	res = cutData(res, 32 - data_size);
	resDetCF = cutData(resDetCF, 32 - data_size);
	cpu.eflags.CF = (resDetCF >> (data_size - 1));
	cpu.eflags.SF = (res >> (data_size - 1));
	cpu.eflags.PF = setPF(res);
	cpu.eflags.ZF = (res == 0) ? 1 : 0;
	return res;
#endif
}
