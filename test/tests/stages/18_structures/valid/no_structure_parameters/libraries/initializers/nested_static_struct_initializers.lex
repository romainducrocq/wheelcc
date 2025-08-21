-- Lexing ... OK
+
+
@@ Tokens @@
List[737]:
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
  struct
  identifier(inner)
  {
  int
  identifier(one_i)
  ;
  signed
  char
  identifier(two_arr)
  [
  const int(3)
  ]
  ;
  unsigned
  identifier(three_u)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  long
  identifier(one_l)
  ;
  struct
  identifier(inner)
  identifier(two_struct)
  ;
  char
  *
  identifier(three_msg)
  ;
  double
  identifier(four_d)
  ;
  }
  ;
  extern
  struct
  identifier(outer)
  identifier(all_zeros)
  ;
  extern
  struct
  identifier(outer)
  identifier(partial)
  ;
  extern
  struct
  identifier(outer)
  identifier(full)
  ;
  extern
  struct
  identifier(outer)
  identifier(converted)
  ;
  extern
  struct
  identifier(outer)
  identifier(struct_array)
  [
  const int(3)
  ]
  ;
  int
  identifier(test_uninitialized)
  (
  void
  )
  ;
  int
  identifier(test_partially_initialized)
  (
  void
  )
  ;
  int
  identifier(test_fully_intialized)
  (
  void
  )
  ;
  int
  identifier(test_implicit_conversions)
  (
  void
  )
  ;
  int
  identifier(test_array_of_structs)
  (
  void
  )
  ;
  int
  identifier(test_uninitialized)
  (
  void
  )
  {
  if
  (
  identifier(all_zeros)
  .
  identifier(one_l)
  ||
  identifier(all_zeros)
  .
  identifier(three_msg)
  ||
  identifier(all_zeros)
  .
  identifier(four_d)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(all_zeros)
  .
  identifier(two_struct)
  .
  identifier(one_i)
  ||
  identifier(all_zeros)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  ||
  identifier(all_zeros)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  ||
  identifier(all_zeros)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  ||
  identifier(all_zeros)
  .
  identifier(two_struct)
  .
  identifier(three_u)
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
  identifier(test_partially_initialized)
  (
  void
  )
  {
  if
  (
  identifier(partial)
  .
  identifier(one_l)
  !=
  const long(100l)
  ||
  identifier(strcmp)
  (
  identifier(partial)
  .
  identifier(three_msg)
  ,
  string literal("Hello!")
  )
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(partial)
  .
  identifier(four_d)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(partial)
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(10)
  ||
  identifier(partial)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(10)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(partial)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  ||
  identifier(partial)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  ||
  identifier(partial)
  .
  identifier(two_struct)
  .
  identifier(three_u)
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
  identifier(test_fully_intialized)
  (
  void
  )
  {
  if
  (
  identifier(full)
  .
  identifier(one_l)
  !=
  const long(18014398509481979l)
  ||
  identifier(strcmp)
  (
  identifier(full)
  .
  identifier(three_msg)
  ,
  string literal("Another message")
  )
  ||
  identifier(full)
  .
  identifier(four_d)
  !=
  const double(2e12)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(full)
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(1000)
  ||
  identifier(full)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const char('o')
  ||
  identifier(full)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const char('k')
  ||
  identifier(full)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const int(0)
  ||
  identifier(full)
  .
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const unsigned int(4292870144u)
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
  identifier(test_implicit_conversions)
  (
  void
  )
  {
  if
  (
  identifier(converted)
  .
  identifier(one_l)
  !=
  const long(10l)
  ||
  identifier(converted)
  .
  identifier(three_msg)
  !=
  const int(0)
  ||
  identifier(converted)
  .
  identifier(four_d)
  !=
  const double(9223372036854777856.0)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(converted)
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  -
  const int(2147483646)
  ||
  identifier(converted)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(15)
  ||
  identifier(converted)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const int(3)
  ||
  identifier(converted)
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  -
  const int(127)
  ||
  identifier(converted)
  .
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const unsigned int(2147483648u)
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
  identifier(test_array_of_structs)
  (
  void
  )
  {
  if
  (
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(one_l)
  !=
  const int(1)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(three_msg)
  !=
  const int(0)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(four_d)
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
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(2)
  ||
  identifier(strcmp)
  (
  (
  char
  *
  )
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  ,
  string literal("ab")
  )
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const int(3)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(one_l)
  !=
  const int(6)
  ||
  identifier(strcmp)
  (
  (
  char
  *
  )
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(three_msg)
  ,
  string literal("Message")
  )
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(four_d)
  !=
  const int(9)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(7)
  ||
  identifier(strcmp)
  (
  (
  char
  *
  )
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  ,
  string literal("cd")
  )
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const int(8)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(one_l)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(three_msg)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(four_d)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(three_u)
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
