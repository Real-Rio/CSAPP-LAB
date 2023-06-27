/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
  // 只能使用“与”和“取反”，要发现异或的规则中蕴含的规律，只有两个数不一样异或为1.因此一个数取反后与另一个数若为1，说明这两个bit不同
  int a3;
  int a1 = (~x) & y;
  int a2 = x & (~y);
  a1 = ~a1;
  a2 = ~a2;
  a3 = a1 & a2;
  return ~a3;
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
  int a = 1;
  return a << 31;
}
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
  // 检查是否为0111111类型，1、判断除了第一位后面的位是否为1，x+x即是左移1位，再将新移入的位置1、如果符合，那么取反后每一位应为0，否则为非0.此时再用!输出返回值。2、还要判断是不是1111111类型，前半部分即是为了检查这种情况。只要判断取反
  // 后是不是为0即可。之所以用两个!是因为!运算符的结果只有0和1，方便与后半部分进行与运算。3、与在这起到条件判断作用，若前半部分为0，则返回值为0，若前半部分为1，返回值由后半部分决定。
  return !!(~x) & !(~(x + x + 1));
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
  // 将偶数位全部置1，再判断全部取反后是否为0
  int mask = 85; // 01010101
  x |= mask;
  mask <<= 8;
  x |= mask;
  mask <<= 8;
  x |= mask;
  mask <<= 8;
  x |= mask;
  return !(~x);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
  // 按位取反后加1则为相反数
  return (~x) + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
  // 0011 0000
  // 0011 1001 如果第3位为1，那么第1和第2位一定为0
  int latter = x ^ 48;                                   // 0011 0000 用来判断高4位是否为0011
  int latter2 = latter & 6;                              // 0110
  return (!(latter >> 4)) & (!(latter >> 3) | !latter2); // latter >> 3判断第3位是否为0，若是则符合；若第3位为1，则要判断第1和第2位是否为0，通过&6将第1和第2位提取出来
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
  // 借鉴了https://wdxtub.com/csapp/thick-csapp-lab-1/2016/04/16/中思路，返回值形式应该为 (y op exp1) | (z op exp2)形式，根据x的值，exp1和exp2一个为0xffffffff，一个为0x0000000
  int mask = (~(!x)) + 1; // x=0 -> mask=0xffffffff x!=0 -> mask=0x00000000
  return (z & mask) | (y & (~mask));
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
  // signXOR判断x和y符号是否一样，一样为0，不一样为1
  // return第一部分判断若x和y符号不一样，结果与x的符号一致；第二部分判断若x和y的符号一致，结果与res的符号一致；第三部分用来判断x和y相等的情况。至于为什么不能直接用x-y判断是因为若二者异号，直接相减会溢出。
  int signX = x >> 31 & 0x1, signY = y >> 31 & 0x1;
  int signXOR = signX ^ signY;
  int res = x + (~y) + 1;
  return (signXOR & signX) | (!signXOR & res >> 31 & 0x1) | !res;
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x)
{
  // 判断一个数是否为0只需要加上0xffffffff判断是否变号,要求符号位为0.比如0x80000001+0xfffffff最高位仍为1，故对这种情况单独处理
  int flag = x + ~0;
  int sign = flag >> 31 & 0x1;
  int sign2 = x >> 31 & 0x1; /* 关键是如何判断sign2是否为0，因为不能直接(!sign2)&sign，只能根据下表的规则使用小trick  */
  return ~sign2 & sign;      // sign2 sign output
                             //   1          0
                             //   0     0    0
                             //   0     1    1
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
  // 转载自https://zhuanlan.zhihu.com/p/59534845
  int b16, b8, b4, b2, b1, b0;
  int sign = x >> 31;
  x = (sign & ~x) | (~sign & x); // 如果x为正则不变，否则按位取反（这样好找最高位为1的，原来是最高位为0的，这样也将符号位去掉了）

  // 不断缩小范围
  b16 = !!(x >> 16) << 4; // 高十六位是否有1
  x = x >> b16;           // 如果有（至少需要16位），则将原数右移16位
  b8 = !!(x >> 8) << 3;   // 剩余位高8位是否有1
  x = x >> b8;            // 如果有（至少需要16+8=24位），则右移8位
  b4 = !!(x >> 4) << 2;   // 同理
  x = x >> b4;
  b2 = !!(x >> 2) << 1;
  x = x >> b2;
  b1 = !!(x >> 1);
  x = x >> b1;
  b0 = x;
  return b16 + b8 + b4 + b2 + b1 + b0 + 1; //+1表示加上符号位
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
  int mask = uf >> 23 & 0xff;
  int tail = uf & 0x7fffff;
  int sign = uf & 0x80000000;
  if (mask == 255 | (!mask && !tail))
    return uf;
  if (!mask && tail)
    return uf << 1 | sign;
  return uf += 0x800000;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
  int exp = (uf >> 23) & 0xFF;
  int frac = uf & 0x007FFFFF;
  int sign = uf & 0x80000000;
  int bias = exp - 127;

  if (exp == 255 || bias > 30)
  {
    // exponent is 255 (NaN), or number is too large for an int
    return 0x80000000u;
  }
  else if (!exp || bias < 0)
  {
    // number is very small, round down to 0
    return 0;
  }

  // append a 1 to the front to normalize
  frac = frac | 1 << 23;

  // float based on the bias
  if (bias > 23)
  {
    frac = frac << (bias - 23);
  }
  else
  {
    frac = frac >> (23 - bias);
  }

  if (sign)
  {
    // original number was negative, make the new number negative
    frac = ~(frac) + 1;
  }

  return frac;
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
  // https://zhuanlan.zhihu.com/p/339047608
   if(x>127){
        return 0xFF<<23;
    }
    else if(x<-149)return 0;
    else if(x>=-126){
        int exp = x + 127;
        return (exp << 23);
    } else{
        int t = 149 + x;
        return (1 << t);
    }
}
