// This may look like C code, but it is really -*- C++ -*-

/* --------------------------------------------------------------------	*
 *	Integer.h			17-sep-1998  by ODF		*
 *                               Revised 2012/01/29  by ODF             *
 * --------------------------------------------------------------------	*/


#ifndef _Integer_h
#define _Integer_h 1


#include <stddef.h>
#include "gmp.h"
#include "shared_array.hpp"


// ------------------------------------------------------------------------

/*

  The class 'Integer' provides a simple wrapper for most of the
  integer routines in the GNU Multiple Precision Arithmetic Library.

*/


class Integer
{
  friend class Rational;

private:
  mpz_t rep;
public:
  Integer() {
    mpz_init(rep);
  }

  Integer(const Integer& n) {
    mpz_init_set(rep, n.rep);
  }

  Integer(int n) {
    mpz_init_set_si(rep, static_cast<long>(n));
  }

  Integer(unsigned int n) {
    mpz_init_set_ui(rep, static_cast<unsigned long>(n));
  }

  Integer(long n) {
    mpz_init_set_si(rep, n);
  }

  Integer(unsigned long n) {
    mpz_init_set_ui(rep, n);
  }

  explicit
  Integer(double n) {
    mpz_init_set_d(rep, n);
  }

  explicit
  Integer(const char* str, int base = 10) {
    mpz_init_set_str(rep, str, base);
  }

  ~Integer()			{ mpz_clear(rep); }


  const Integer&
  operator = (const Integer& n)	{ mpz_set(rep, n.rep); return *this; }

// unary operations to self

  const Integer&
  operator ++ ()		{ mpz_add_ui(rep, rep, 1L); return *this; }

  const Integer&
  operator -- ()		{ mpz_sub_ui(rep, rep, 1L); return *this; }

  Integer
  operator ++ (int)
  { Integer r = *this; mpz_add_ui(rep, rep, 1L); return r; }

  Integer
  operator -- (int)
  { Integer r = *this; mpz_sub_ui(rep, rep, 1L); return r; }

// assignment-based operations

  const Integer&
  operator += (const Integer& op) {
    mpz_add(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator -= (const Integer& op) {
    mpz_sub(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator *= (const Integer& op) {
    mpz_mul(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator /= (const Integer& op) {
    mpz_tdiv_q(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator %= (const Integer& op) {
    mpz_tdiv_r(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator &= (const Integer& op) {
    mpz_and(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator |= (const Integer& op) {
    mpz_ior(rep, rep, op.rep); return *this;
  }

  const Integer&
  operator <<= (long int op) {
    if (op < 0)
      mpz_tdiv_q_2exp(rep, rep, static_cast<unsigned long>(-op));
    else if (op > 0)
      mpz_mul_2exp(rep, rep, static_cast<unsigned long>(op));
    return *this;
  }

  const Integer&
  operator >>= (long int op) {
    if (op < 0)
      mpz_mul_2exp(rep, rep, static_cast<unsigned long>(-op));
    else if (op > 0)
      mpz_tdiv_q_2exp(rep, rep, static_cast<unsigned long>(op));
    return *this;
  }


// Comparison

  int
  compare(const Integer& rop) const {
    return mpz_cmp(rep, rop.rep);
  }


// Arithmetic operators

  Integer
  operator -  () const {
    Integer r; mpz_neg(r.rep, rep); return r;
  }

  Integer
  operator ~  () const {
    Integer r; mpz_com(r.rep, rep); return r;
  }

  friend Integer
  operator +  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_add(r.rep, lop.rep, rop.rep); return r;
  }

  friend Integer
  operator -  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_sub(r.rep, lop.rep, rop.rep); return r;
  }

  friend Integer
  operator *  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_mul(r.rep, lop.rep, rop.rep); return r;
  }

  friend Integer
  operator /  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_tdiv_q(r.rep, lop.rep, rop.rep); return r;
  }

  friend Integer
  operator %  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_tdiv_r(r.rep, lop.rep, rop.rep); return r;
  }

// Logical and bitwise operations

  friend Integer
  operator &  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_and(r.rep, lop.rep, rop.rep); return r;
  }

  friend Integer
  operator |  (const Integer& lop, const Integer& rop) {
    Integer r; mpz_ior(r.rep, lop.rep, rop.rep); return r;
  }

  Integer
  operator << (long int op) {
    Integer r;
    if (op < 0)
      mpz_tdiv_q_2exp(r.rep, rep, static_cast<unsigned long>(-op));
    else if (op > 0)
      mpz_mul_2exp(r.rep, rep, static_cast<unsigned long>(op));
    return r;
  }

  Integer
  operator >> (long int op) {
    Integer r;
    if (op < 0)
      mpz_mul_2exp(r.rep, rep, static_cast<unsigned long>(-op));
    else if (op > 0)
      mpz_tdiv_q_2exp(r.rep, rep, static_cast<unsigned long>(op));
    return r;
  }


// miscellaneous functions

  // absolute value

  Integer
  abs() const {
    Integer r; mpz_abs(r.rep, rep); return r;
  }

  // sign

  int
  sgn() const {
    return mpz_sgn(rep);
  }

  // power

  friend Integer
  pow (const Integer& lop, const Integer& rop) {
    mpz_t one;
    mpz_init_set_si(one, 1L);
    Integer r; mpz_powm(r.rep, lop.rep, rop.rep, one); return r;
  }

  // greatest common divisor

  friend Integer
  gcd (const Integer& lop, const Integer& rop) {
    Integer r; mpz_gcd(r.rep, lop.rep, rop.rep); return r;
  }

  // square root

  Integer
  sqrt() const {
    Integer r; mpz_sqrt(r.rep, rep); return r;
  }

  // factorial

  friend Integer
  fac(long int op) {
    Integer r;
    mpz_fac_ui(r.rep, static_cast<unsigned long>(op > 0 ? op : -op));
    return r;
  }

  // conversion

  unsigned long int 
  get_ulong() const {
    return mpz_get_ui(rep);
  }

  long int 
  get_long() const {
    return mpz_get_si(rep);
  }

  double
  get_double() const {
    return mpz_get_d(rep);
  }

  shared_array<char>
  get_string(int base = 10) const {
    shared_array<char> buf(mpz_sizeinbase(rep, base) + 2);
    mpz_get_str(buf.get(), base, rep);
    return buf;
  }

  // output

  void
  print(std::ostream& out) const {
    out << get_string();
  }
};


// ------------------------------------------------------------------------

// Comparison operators:

inline bool
operator == (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) == 0;
}

inline bool
operator != (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) != 0;
}

inline bool
operator < (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) < 0;
}

inline bool
operator <= (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) <= 0;
}

inline bool
operator > (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) > 0;
}

inline bool
operator >= (const Integer& lop, const Integer& rop) {
  return lop.compare(rop) >= 0;
}


// output to a stream

inline std::ostream&
operator<< (std::ostream& out, const Integer& n)
{
  n.print(out);
  return out;
}


// ------------------------------------------------------------------------


#endif /* !_Integer_h */

/* --- EOF Integer.h --- */
