-- Lexing ... OK
+
+
@@ Tokens @@
List[1014]:
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
  void
  identifier(exit)
  (
  int
  identifier(status)
  )
  ;
  struct
  identifier(stack_bytes)
  {
  char
  identifier(bytes)
  [
  const int(16)
  ]
  ;
  }
  ;
  static
  struct
  identifier(stack_bytes)
  identifier(to_validate)
  ;
  void
  identifier(validate_stack_bytes)
  (
  int
  identifier(code)
  )
  {
  if
  (
  identifier(strcmp)
  (
  identifier(to_validate)
  .
  identifier(bytes)
  ,
  string literal("efghijklmnopqrs")
  )
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  return
  ;
  }
  struct
  identifier(one_int_reg)
  {
  char
  identifier(cs)
  [
  const int(7)
  ]
  ;
  }
  ;
  struct
  identifier(one_int_reg)
  identifier(return_int_struct)
  (
  void
  )
  {
  struct
  identifier(one_int_reg)
  identifier(retval)
  =
  {
  {
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  }
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(one_int_reg)
  identifier(one_int_struct)
  ;
  void
  identifier(validate_one_int_struct)
  (
  int
  identifier(code)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(7)
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
  identifier(one_int_struct)
  .
  identifier(cs)
  [
  identifier(i)
  ]
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  }
  int
  identifier(test_int_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(one_int_struct)
  =
  identifier(return_int_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(1)
  )
  ;
  identifier(validate_one_int_struct)
  (
  const int(2)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(two_int_regs)
  {
  char
  identifier(cs)
  [
  const int(15)
  ]
  ;
  }
  ;
  struct
  identifier(two_int_regs)
  identifier(return_two_int_struct)
  (
  void
  )
  {
  struct
  identifier(two_int_regs)
  identifier(retval)
  =
  {
  {
  const int(20)
  ,
  const int(21)
  ,
  const int(22)
  ,
  const int(23)
  ,
  const int(24)
  ,
  const int(25)
  ,
  const int(26)
  ,
  const int(27)
  ,
  const int(28)
  ,
  const int(29)
  ,
  const int(30)
  ,
  const int(31)
  ,
  const int(32)
  ,
  const int(33)
  ,
  const int(34)
  }
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(two_int_regs)
  identifier(two_int_struct)
  ;
  void
  identifier(validate_two_int_struct)
  (
  int
  identifier(code)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(15)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  if
  (
  identifier(two_int_struct)
  .
  identifier(cs)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(20)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  int
  identifier(test_two_int_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(two_int_struct)
  =
  identifier(return_two_int_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(3)
  )
  ;
  identifier(validate_two_int_struct)
  (
  const int(4)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(one_xmm_reg)
  {
  double
  identifier(d)
  ;
  }
  ;
  struct
  identifier(one_xmm_reg)
  identifier(return_one_xmm_struct)
  (
  void
  )
  {
  struct
  identifier(one_xmm_reg)
  identifier(retval)
  =
  {
  const double(234.5)
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(one_xmm_reg)
  identifier(one_double_struct)
  ;
  void
  identifier(validate_one_double_struct)
  (
  int
  identifier(code)
  )
  {
  if
  (
  identifier(one_double_struct)
  .
  identifier(d)
  !=
  const double(234.5)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  int
  identifier(test_one_double_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(one_double_struct)
  =
  identifier(return_one_xmm_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(5)
  )
  ;
  identifier(validate_one_double_struct)
  (
  const int(6)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(two_xmm_regs)
  {
  double
  identifier(d1)
  ;
  double
  identifier(d2)
  ;
  }
  ;
  struct
  identifier(two_xmm_regs)
  identifier(return_two_xmm_struct)
  (
  void
  )
  {
  struct
  identifier(two_xmm_regs)
  identifier(retval)
  =
  {
  const double(234.5)
  ,
  const double(678.25)
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(two_xmm_regs)
  identifier(two_doubles_struct)
  ;
  void
  identifier(validate_two_doubles_struct)
  (
  int
  identifier(code)
  )
  {
  if
  (
  identifier(two_doubles_struct)
  .
  identifier(d1)
  !=
  const double(234.5)
  ||
  identifier(two_doubles_struct)
  .
  identifier(d2)
  !=
  const double(678.25)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  int
  identifier(test_two_doubles_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(two_doubles_struct)
  =
  identifier(return_two_xmm_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(7)
  )
  ;
  identifier(validate_two_doubles_struct)
  (
  const int(8)
  )
  ;
  return
  const int(0)
  ;
  }
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
  identifier(int_and_xmm)
  identifier(return_mixed_struct)
  (
  void
  )
  {
  struct
  identifier(int_and_xmm)
  identifier(retval)
  =
  {
  const int(125)
  ,
  const double(678.25)
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(int_and_xmm)
  identifier(mixed_struct)
  ;
  void
  identifier(validate_mixed_struct)
  (
  int
  identifier(code)
  )
  {
  if
  (
  identifier(mixed_struct)
  .
  identifier(c)
  !=
  const int(125)
  ||
  identifier(mixed_struct)
  .
  identifier(d)
  !=
  const double(678.25)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  int
  identifier(test_mixed_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(mixed_struct)
  =
  identifier(return_mixed_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(9)
  )
  ;
  identifier(validate_mixed_struct)
  (
  const int(10)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(stack)
  {
  char
  identifier(cs)
  [
  const int(28)
  ]
  ;
  }
  ;
  struct
  identifier(stack)
  identifier(return_stack_struct)
  (
  void
  )
  {
  struct
  identifier(stack)
  identifier(retval)
  =
  {
  {
  const int(90)
  ,
  const int(91)
  ,
  const int(92)
  ,
  const int(93)
  ,
  const int(94)
  ,
  const int(95)
  ,
  const int(96)
  ,
  const int(97)
  ,
  const int(98)
  ,
  const int(99)
  ,
  const int(100)
  ,
  const int(101)
  ,
  const int(102)
  ,
  const int(103)
  ,
  const int(104)
  ,
  const int(105)
  ,
  const int(106)
  ,
  const int(107)
  ,
  const int(108)
  ,
  const int(109)
  ,
  const int(110)
  ,
  const int(111)
  ,
  const int(112)
  ,
  const int(113)
  ,
  const int(114)
  ,
  const int(115)
  ,
  const int(116)
  ,
  const int(117)
  }
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(stack)
  identifier(stack_struct)
  ;
  void
  identifier(validate_stack_struct)
  (
  int
  identifier(code)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(28)
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
  identifier(stack_struct)
  .
  identifier(cs)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(90)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  }
  int
  identifier(test_stack_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(stack_struct)
  =
  identifier(return_stack_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(11)
  )
  ;
  identifier(validate_stack_struct)
  (
  const int(12)
  )
  ;
  return
  const int(0)
  ;
  }
  struct
  identifier(stack_irregular)
  {
  char
  identifier(cs)
  [
  const int(19)
  ]
  ;
  }
  ;
  struct
  identifier(stack_irregular)
  identifier(return_irregular_stack_struct)
  (
  void
  )
  {
  struct
  identifier(stack_irregular)
  identifier(retval)
  =
  {
  {
  const int(70)
  ,
  const int(71)
  ,
  const int(72)
  ,
  const int(73)
  ,
  const int(74)
  ,
  const int(75)
  ,
  const int(76)
  ,
  const int(77)
  ,
  const int(78)
  ,
  const int(79)
  ,
  const int(80)
  ,
  const int(81)
  ,
  const int(82)
  ,
  const int(83)
  ,
  const int(84)
  ,
  const int(85)
  ,
  const int(86)
  ,
  const int(87)
  ,
  const int(88)
  }
  }
  ;
  return
  identifier(retval)
  ;
  }
  static
  struct
  identifier(stack_irregular)
  identifier(irregular_stack_struct)
  ;
  void
  identifier(validate_irregular_stack_struct)
  (
  int
  identifier(code)
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(19)
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
  identifier(irregular_stack_struct)
  .
  identifier(cs)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(70)
  )
  {
  identifier(exit)
  (
  identifier(code)
  )
  ;
  }
  }
  }
  int
  identifier(test_irregular_stack_struct)
  (
  void
  )
  {
  struct
  identifier(stack_bytes)
  identifier(bytes)
  =
  {
  string literal("efghijklmnopqrs")
  }
  ;
  identifier(irregular_stack_struct)
  =
  identifier(return_irregular_stack_struct)
  (
  )
  ;
  identifier(to_validate)
  =
  identifier(bytes)
  ;
  identifier(validate_stack_bytes)
  (
  const int(13)
  )
  ;
  identifier(validate_irregular_stack_struct)
  (
  const int(14)
  )
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(test_int_struct)
  (
  )
  ;
  identifier(test_two_int_struct)
  (
  )
  ;
  identifier(test_one_double_struct)
  (
  )
  ;
  identifier(test_two_doubles_struct)
  (
  )
  ;
  identifier(test_mixed_struct)
  (
  )
  ;
  identifier(test_stack_struct)
  (
  )
  ;
  identifier(test_irregular_stack_struct)
  (
  )
  ;
  return
  const int(0)
  ;
  }
