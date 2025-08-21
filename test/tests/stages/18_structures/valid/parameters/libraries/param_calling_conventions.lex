-- Lexing ... OK
+
+
@@ Tokens @@
List[1380]:
  int
  identifier(strcmp)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
  )
  ;
  int
  identifier(strncmp)
  (
  char
  *
  identifier(s1)
  ,
  char
  *
  identifier(s2)
  ,
  unsigned
  long
  identifier(n)
  )
  ;
  struct
  identifier(two_longs)
  {
  long
  identifier(a)
  ;
  long
  identifier(b)
  ;
  }
  ;
  struct
  identifier(one_int)
  {
  int
  identifier(i)
  ;
  char
  identifier(c)
  ;
  }
  ;
  struct
  identifier(one_int_exactly)
  {
  unsigned
  long
  identifier(l)
  ;
  }
  ;
  struct
  identifier(two_ints)
  {
  char
  identifier(c)
  ;
  int
  identifier(arr)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(two_ints_nested)
  {
  struct
  identifier(one_int)
  identifier(a)
  ;
  struct
  identifier(one_int)
  identifier(b)
  ;
  }
  ;
  struct
  identifier(twelve_bytes)
  {
  int
  identifier(i)
  ;
  char
  identifier(arr)
  [
  const int(8)
  ]
  ;
  }
  ;
  struct
  identifier(one_xmm)
  {
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(two_xmm)
  {
  double
  identifier(d)
  [
  const int(2)
  ]
  ;
  }
  ;
  struct
  identifier(int_and_xmm)
  {
  char
  identifier(c)
  ;
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(xmm_and_int)
  {
  struct
  identifier(one_xmm)
  identifier(dbl)
  ;
  char
  identifier(c)
  [
  const int(3)
  ]
  ;
  }
  ;
  struct
  identifier(odd_size)
  {
  char
  identifier(arr)
  [
  const int(5)
  ]
  ;
  }
  ;
  struct
  identifier(memory)
  {
  double
  identifier(d)
  ;
  char
  identifier(c)
  [
  const int(3)
  ]
  ;
  long
  identifier(l)
  ;
  int
  identifier(i)
  ;
  }
  ;
  int
  identifier(pass_small_structs)
  (
  struct
  identifier(two_xmm)
  identifier(two_xmm_struct)
  ,
  struct
  identifier(one_int)
  identifier(int_struct)
  ,
  struct
  identifier(one_xmm)
  identifier(xmm_struct)
  ,
  struct
  identifier(xmm_and_int)
  identifier(mixed_struct)
  ,
  struct
  identifier(twelve_bytes)
  identifier(int_struct_2)
  ,
  struct
  identifier(one_int_exactly)
  identifier(another_int_struct)
  )
  ;
  int
  identifier(a_bunch_of_arguments)
  (
  int
  identifier(i0)
  ,
  int
  identifier(i1)
  ,
  int
  identifier(i2)
  ,
  int
  identifier(i3)
  ,
  int
  identifier(i4)
  ,
  struct
  identifier(two_longs)
  identifier(param)
  ,
  int
  identifier(i5)
  )
  ;
  int
  identifier(structs_and_scalars)
  (
  long
  identifier(l)
  ,
  double
  identifier(d)
  ,
  struct
  identifier(odd_size)
  identifier(os)
  ,
  struct
  identifier(memory)
  identifier(mem)
  ,
  struct
  identifier(one_xmm)
  identifier(xmm_struct)
  )
  ;
  int
  identifier(struct_in_mem)
  (
  double
  identifier(a)
  ,
  double
  identifier(b)
  ,
  double
  identifier(c)
  ,
  struct
  identifier(xmm_and_int)
  identifier(first_struct)
  ,
  double
  identifier(d)
  ,
  struct
  identifier(two_xmm)
  identifier(second_struct)
  ,
  long
  identifier(l)
  ,
  struct
  identifier(int_and_xmm)
  identifier(third_struct)
  ,
  struct
  identifier(one_xmm)
  identifier(fourth_struct)
  )
  ;
  int
  identifier(pass_borderline_struct_in_memory)
  (
  struct
  identifier(two_ints)
  identifier(t_i)
  ,
  int
  identifier(c)
  ,
  struct
  identifier(int_and_xmm)
  identifier(i_x)
  ,
  void
  *
  identifier(ptr)
  ,
  struct
  identifier(two_ints_nested)
  identifier(t_i_n)
  ,
  double
  identifier(d)
  )
  ;
  int
  identifier(pass_uneven_struct_in_mem)
  (
  struct
  identifier(twelve_bytes)
  identifier(struct1)
  ,
  long
  identifier(a)
  ,
  long
  identifier(b)
  ,
  struct
  identifier(twelve_bytes)
  identifier(struct2)
  ,
  struct
  identifier(odd_size)
  identifier(os)
  ,
  struct
  identifier(memory)
  identifier(m)
  )
  ;
  int
  identifier(pass_later_structs_in_regs)
  (
  struct
  identifier(memory)
  identifier(m)
  ,
  struct
  identifier(twelve_bytes)
  identifier(struct1)
  ,
  struct
  identifier(one_xmm)
  identifier(struct2)
  )
  ;
  int
  identifier(pass_small_structs)
  (
  struct
  identifier(two_xmm)
  identifier(two_xmm_struct)
  ,
  struct
  identifier(one_int)
  identifier(int_struct)
  ,
  struct
  identifier(one_xmm)
  identifier(xmm_struct)
  ,
  struct
  identifier(xmm_and_int)
  identifier(mixed_struct)
  ,
  struct
  identifier(twelve_bytes)
  identifier(int_struct_2)
  ,
  struct
  identifier(one_int_exactly)
  identifier(another_int_struct)
  )
  {
  if
  (
  identifier(two_xmm_struct)
  .
  identifier(d)
  [
  const int(0)
  ]
  !=
  const double(55.5)
  ||
  identifier(two_xmm_struct)
  .
  identifier(d)
  [
  const int(1)
  ]
  !=
  const double(44.4)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(int_struct)
  .
  identifier(c)
  !=
  const char('c')
  ||
  identifier(int_struct)
  .
  identifier(i)
  !=
  const int(54320)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(xmm_struct)
  .
  identifier(d)
  !=
  const double(5.125)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(mixed_struct)
  .
  identifier(c)
  ,
  string literal("hi")
  )
  ||
  identifier(mixed_struct)
  .
  identifier(dbl)
  .
  identifier(d)
  !=
  const double(1.234)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(int_struct_2)
  .
  identifier(arr)
  ,
  string literal("string!")
  )
  ||
  identifier(int_struct_2)
  .
  identifier(i)
  !=
  const int(123)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(another_int_struct)
  .
  identifier(l)
  !=
  const int(567890)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
  int
  identifier(a_bunch_of_arguments)
  (
  int
  identifier(i0)
  ,
  int
  identifier(i1)
  ,
  int
  identifier(i2)
  ,
  int
  identifier(i3)
  ,
  int
  identifier(i4)
  ,
  struct
  identifier(two_longs)
  identifier(param)
  ,
  int
  identifier(i5)
  )
  {
  if
  (
  identifier(i0)
  !=
  const int(0)
  ||
  identifier(i1)
  !=
  const int(1)
  ||
  identifier(i2)
  !=
  const int(2)
  ||
  identifier(i3)
  !=
  const int(3)
  ||
  identifier(i4)
  !=
  const int(4)
  ||
  identifier(i5)
  !=
  const int(5)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(param)
  .
  identifier(a)
  !=
  const long(1234567l)
  ||
  identifier(param)
  .
  identifier(b)
  !=
  const long(89101112l)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(structs_and_scalars)
  (
  long
  identifier(l)
  ,
  double
  identifier(d)
  ,
  struct
  identifier(odd_size)
  identifier(os)
  ,
  struct
  identifier(memory)
  identifier(mem)
  ,
  struct
  identifier(one_xmm)
  identifier(xmm_struct)
  )
  {
  if
  (
  identifier(l)
  !=
  const int(10)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(d)
  !=
  const double(10.0)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(os)
  .
  identifier(arr)
  ,
  string literal("lmno")
  )
  )
  return
  const int(0)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(mem)
  .
  identifier(c)
  ,
  string literal("rs")
  )
  ||
  identifier(mem)
  .
  identifier(d)
  !=
  const double(15.75)
  ||
  identifier(mem)
  .
  identifier(i)
  !=
  const int(3333)
  ||
  identifier(mem)
  .
  identifier(l)
  !=
  const int(4444)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(xmm_struct)
  .
  identifier(d)
  !=
  const double(5.125)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
  int
  identifier(struct_in_mem)
  (
  double
  identifier(a)
  ,
  double
  identifier(b)
  ,
  double
  identifier(c)
  ,
  struct
  identifier(xmm_and_int)
  identifier(first_struct)
  ,
  double
  identifier(d)
  ,
  struct
  identifier(two_xmm)
  identifier(second_struct)
  ,
  long
  identifier(l)
  ,
  struct
  identifier(int_and_xmm)
  identifier(third_struct)
  ,
  struct
  identifier(one_xmm)
  identifier(fourth_struct)
  )
  {
  if
  (
  identifier(a)
  !=
  const double(10.0)
  ||
  identifier(b)
  !=
  const double(11.125)
  ||
  identifier(c)
  !=
  const double(12.0)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(strcmp)
  (
  identifier(first_struct)
  .
  identifier(c)
  ,
  string literal("hi")
  )
  ||
  identifier(first_struct)
  .
  identifier(dbl)
  .
  identifier(d)
  !=
  const double(1.234)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(d)
  !=
  const double(13.0)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(second_struct)
  .
  identifier(d)
  [
  const int(0)
  ]
  !=
  const double(55.5)
  ||
  identifier(second_struct)
  .
  identifier(d)
  [
  const int(1)
  ]
  !=
  const double(44.4)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(l)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(third_struct)
  .
  identifier(c)
  !=
  const char('p')
  ||
  identifier(third_struct)
  .
  identifier(d)
  !=
  const double(4.56)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(fourth_struct)
  .
  identifier(d)
  !=
  const double(5.125)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
  int
  identifier(pass_borderline_struct_in_memory)
  (
  struct
  identifier(two_ints)
  identifier(t_i)
  ,
  int
  identifier(c)
  ,
  struct
  identifier(int_and_xmm)
  identifier(i_x)
  ,
  void
  *
  identifier(ptr)
  ,
  struct
  identifier(two_ints_nested)
  identifier(t_i_n)
  ,
  double
  identifier(d)
  )
  {
  if
  (
  identifier(t_i)
  .
  identifier(c)
  !=
  const char('_')
  ||
  identifier(t_i)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(5)
  ||
  identifier(t_i)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(6)
  ||
  identifier(t_i)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(7)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(c)
  !=
  const char('!')
  )
  return
  const int(0)
  ;
  if
  (
  identifier(i_x)
  .
  identifier(c)
  !=
  const char('p')
  ||
  identifier(i_x)
  .
  identifier(d)
  !=
  const double(4.56)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(ptr)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(t_i_n)
  .
  identifier(a)
  .
  identifier(c)
  !=
  const char('c')
  ||
  identifier(t_i_n)
  .
  identifier(a)
  .
  identifier(i)
  !=
  const int(54320)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(t_i_n)
  .
  identifier(b)
  .
  identifier(c)
  !=
  const char('c')
  ||
  identifier(t_i_n)
  .
  identifier(b)
  .
  identifier(i)
  !=
  const int(54320)
  )
  return
  const int(0)
  ;
  if
  (
  identifier(d)
  !=
  const double(7.8)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
  int
  identifier(pass_uneven_struct_in_mem)
  (
  struct
  identifier(twelve_bytes)
  identifier(struct1)
  ,
  long
  identifier(a)
  ,
  long
  identifier(b)
  ,
  struct
  identifier(twelve_bytes)
  identifier(struct2)
  ,
  struct
  identifier(odd_size)
  identifier(os)
  ,
  struct
  identifier(memory)
  identifier(m)
  )
  {
  if
  (
  identifier(struct1)
  .
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(127)
  ||
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(126)
  ||
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(125)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(a)
  !=
  const long(9223372036854775805l)
  ||
  identifier(b)
  !=
  const long(9223372036854775800l)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct2)
  .
  identifier(i)
  !=
  -
  const int(5)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct2)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(100)
  ||
  identifier(struct2)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(101)
  ||
  identifier(struct2)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(102)
  )
  {
  return
  const int(0)
  ;
  }
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(5)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(os)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  const int(100)
  -
  identifier(i)
  )
  {
  return
  const int(0)
  ;
  }
  }
  if
  (
  identifier(m)
  .
  identifier(d)
  !=
  const double(5.345)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(c)
  [
  const int(0)
  ]
  !=
  -
  const int(1)
  ||
  identifier(m)
  .
  identifier(c)
  [
  const int(1)
  ]
  !=
  -
  const int(2)
  ||
  identifier(m)
  .
  identifier(c)
  [
  const int(2)
  ]
  !=
  -
  const int(3)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(l)
  !=
  const long(4294967300l)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(i)
  !=
  const int(10000)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(pass_later_structs_in_regs)
  (
  struct
  identifier(memory)
  identifier(m)
  ,
  struct
  identifier(twelve_bytes)
  identifier(struct1)
  ,
  struct
  identifier(one_xmm)
  identifier(struct2)
  )
  {
  if
  (
  identifier(m)
  .
  identifier(d)
  !=
  const double(5.345)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(c)
  [
  const int(0)
  ]
  !=
  -
  const int(1)
  ||
  identifier(m)
  .
  identifier(c)
  [
  const int(1)
  ]
  !=
  -
  const int(2)
  ||
  identifier(m)
  .
  identifier(c)
  [
  const int(2)
  ]
  !=
  -
  const int(3)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(l)
  !=
  const long(4294967300l)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(m)
  .
  identifier(i)
  !=
  const int(10000)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct1)
  .
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(127)
  ||
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(126)
  ||
  identifier(struct1)
  .
  identifier(arr)
  [
  const int(2)
  ]
  !=
  const int(125)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct2)
  .
  identifier(d)
  !=
  const double(5.125)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
