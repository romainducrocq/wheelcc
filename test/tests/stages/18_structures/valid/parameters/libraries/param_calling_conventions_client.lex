-- Lexing ... OK
+
+
@@ Tokens @@
List[729]:
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
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(two_longs)
  identifier(two_longs)
  =
  {
  const long(1234567l)
  ,
  const long(89101112l)
  }
  ;
  struct
  identifier(one_int)
  identifier(one_int)
  =
  {
  const int(54320)
  ,
  const char('c')
  }
  ;
  struct
  identifier(one_int_exactly)
  identifier(one_long)
  =
  {
  const long(567890l)
  }
  ;
  struct
  identifier(two_ints)
  identifier(two_ints)
  =
  {
  const char('_')
  ,
  {
  const int(5)
  ,
  const int(6)
  ,
  const int(7)
  }
  }
  ;
  struct
  identifier(two_ints_nested)
  identifier(two_ints_nested)
  =
  {
  identifier(one_int)
  ,
  identifier(one_int)
  }
  ;
  struct
  identifier(twelve_bytes)
  identifier(xii)
  =
  {
  const int(123)
  ,
  string literal("string!")
  }
  ;
  struct
  identifier(one_xmm)
  identifier(one_xmm)
  =
  {
  const double(5.125)
  }
  ;
  struct
  identifier(two_xmm)
  identifier(two_xmm)
  =
  {
  {
  const double(55.5)
  ,
  const double(44.4)
  }
  }
  ;
  struct
  identifier(int_and_xmm)
  identifier(int_and_xmm)
  =
  {
  const char('p')
  ,
  const double(4.56)
  }
  ;
  struct
  identifier(xmm_and_int)
  identifier(xmm_and_int)
  =
  {
  {
  const double(1.234)
  }
  ,
  string literal("hi")
  }
  ;
  struct
  identifier(odd_size)
  identifier(odd)
  =
  {
  string literal("lmno")
  }
  ;
  struct
  identifier(memory)
  identifier(mem)
  =
  {
  const double(15.75)
  ,
  string literal("rs")
  ,
  const int(4444)
  ,
  const int(3333)
  }
  ;
  if
  (
  !
  identifier(pass_small_structs)
  (
  identifier(two_xmm)
  ,
  identifier(one_int)
  ,
  identifier(one_xmm)
  ,
  identifier(xmm_and_int)
  ,
  identifier(xii)
  ,
  identifier(one_long)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(a_bunch_of_arguments)
  (
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  identifier(two_longs)
  ,
  const int(5)
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(structs_and_scalars)
  (
  const int(10)
  ,
  const double(10.0)
  ,
  identifier(odd)
  ,
  identifier(mem)
  ,
  identifier(one_xmm)
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(struct_in_mem)
  (
  const double(10.0)
  ,
  const double(11.125)
  ,
  const double(12.0)
  ,
  identifier(xmm_and_int)
  ,
  const double(13.0)
  ,
  identifier(two_xmm)
  ,
  const int(0)
  ,
  identifier(int_and_xmm)
  ,
  identifier(one_xmm)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(pass_borderline_struct_in_memory)
  (
  identifier(two_ints)
  ,
  const char('!')
  ,
  identifier(int_and_xmm)
  ,
  const int(0)
  ,
  identifier(two_ints_nested)
  ,
  const double(7.8)
  )
  )
  {
  return
  const int(4)
  ;
  }
  struct
  identifier(twelve_bytes)
  identifier(struct1)
  =
  {
  -
  const int(1)
  ,
  {
  const int(127)
  ,
  const int(126)
  ,
  const int(125)
  }
  }
  ;
  struct
  identifier(twelve_bytes)
  identifier(struct2)
  =
  {
  -
  const int(5)
  ,
  {
  const int(100)
  ,
  const int(101)
  ,
  const int(102)
  }
  }
  ;
  struct
  identifier(odd_size)
  identifier(os)
  =
  {
  {
  const int(100)
  ,
  const int(99)
  ,
  const int(98)
  ,
  const int(97)
  ,
  const int(96)
  }
  }
  ;
  struct
  identifier(memory)
  identifier(m)
  =
  {
  const double(5.345)
  ,
  {
  -
  const int(1)
  ,
  -
  const int(2)
  ,
  -
  const int(3)
  }
  ,
  const long(4294967300l)
  ,
  const int(10000)
  }
  ;
  if
  (
  !
  identifier(pass_uneven_struct_in_mem)
  (
  identifier(struct1)
  ,
  const long(9223372036854775805l)
  ,
  const long(9223372036854775800l)
  ,
  identifier(struct2)
  ,
  identifier(os)
  ,
  identifier(m)
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(pass_later_structs_in_regs)
  (
  identifier(m)
  ,
  identifier(struct1)
  ,
  identifier(one_xmm)
  )
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
