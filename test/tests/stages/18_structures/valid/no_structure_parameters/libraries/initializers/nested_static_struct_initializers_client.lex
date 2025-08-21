-- Lexing ... OK
+
+
@@ Tokens @@
List[286]:
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
  struct
  identifier(outer)
  identifier(all_zeros)
  ;
  struct
  identifier(outer)
  identifier(partial)
  =
  {
  const long(100l)
  ,
  {
  const int(10)
  ,
  {
  const int(10)
  }
  }
  ,
  string literal("Hello!")
  }
  ;
  struct
  identifier(outer)
  identifier(full)
  =
  {
  const long(18014398509481979l)
  ,
  {
  const int(1000)
  ,
  string literal("ok")
  ,
  const unsigned int(4292870144u)
  }
  ,
  string literal("Another message")
  ,
  const double(2e12)
  }
  ;
  struct
  identifier(outer)
  identifier(converted)
  =
  {
  const double(10.5)
  ,
  {
  const unsigned int(2147483650u)
  ,
  {
  const double(15.6)
  ,
  const long(17592186044419l)
  ,
  const unsigned int(2147483777u)
  }
  ,
  const unsigned long(1152921506754330624ul)
  }
  ,
  const unsigned long(0ul)
  ,
  const unsigned long(9223372036854776833ul)
  }
  ;
  struct
  identifier(outer)
  identifier(struct_array)
  [
  const int(3)
  ]
  =
  {
  {
  const int(1)
  ,
  {
  const int(2)
  ,
  string literal("ab")
  ,
  const int(3)
  }
  ,
  const int(0)
  ,
  const int(5)
  }
  ,
  {
  const int(6)
  ,
  {
  const int(7)
  ,
  string literal("cd")
  ,
  const int(8)
  }
  ,
  string literal("Message")
  ,
  const int(9)
  }
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_uninitialized)
  (
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
  identifier(test_partially_initialized)
  (
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
  identifier(test_fully_intialized)
  (
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
  identifier(test_implicit_conversions)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  !
  identifier(test_array_of_structs)
  (
  )
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
