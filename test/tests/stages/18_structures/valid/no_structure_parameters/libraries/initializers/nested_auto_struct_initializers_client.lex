-- Lexing ... OK
+
+
@@ Tokens @@
List[437]:
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
  identifier(pair)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  struct
  identifier(inner)
  {
  int
  identifier(one_i)
  ;
  unsigned
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
  struct
  identifier(pair)
  identifier(five_pair)
  ;
  }
  ;
  int
  identifier(validate_full_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(validate_partial_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(validate_mixed_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(validate_array_of_structs)
  (
  struct
  identifier(outer)
  *
  identifier(struct_array)
  )
  ;
  int
  identifier(test_full_initialization)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(full)
  =
  {
  -
  const int(200)
  ,
  {
  -
  const long(171l)
  ,
  {
  -
  const int(56)
  ,
  -
  const int(54)
  ,
  -
  const int(53)
  }
  ,
  const double(40.5)
  }
  ,
  string literal("Important message!")
  ,
  -
  const int(22)
  ,
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ;
  return
  identifier(validate_full_initialization)
  (
  &
  identifier(full)
  )
  ;
  }
  int
  identifier(test_partial_initialization)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(partial)
  =
  {
  const int(1000)
  ,
  {
  const int(1)
  ,
  }
  ,
  string literal("Partial")
  }
  ;
  return
  identifier(validate_partial_initialization)
  (
  &
  identifier(partial)
  )
  ;
  }
  int
  identifier(test_mixed_initialization)
  (
  void
  )
  {
  struct
  identifier(inner)
  identifier(inner1)
  =
  {
  const int(10)
  }
  ;
  struct
  identifier(inner)
  identifier(inner2)
  =
  {
  const int(20)
  ,
  {
  const int(21)
  }
  ,
  const unsigned int(22u)
  }
  ;
  static
  int
  identifier(flag)
  =
  const int(0)
  ;
  struct
  identifier(outer)
  identifier(mixed)
  =
  {
  const int(200)
  ,
  identifier(flag)
  ?
  identifier(inner1)
  :
  identifier(inner2)
  ,
  string literal("mixed")
  ,
  const double(10.0)
  ,
  {
  const int(99)
  ,
  const int(100)
  }
  }
  ;
  return
  identifier(validate_mixed_initialization)
  (
  &
  identifier(mixed)
  )
  ;
  }
  int
  identifier(test_array_of_structs)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(s0)
  =
  {
  const int(1)
  ,
  {
  const int(2)
  ,
  {
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  }
  ,
  const int(6)
  }
  ,
  string literal("7")
  ,
  const double(8.0)
  ,
  {
  const int(9)
  ,
  const int(10)
  }
  }
  ;
  struct
  identifier(inner)
  identifier(in1)
  =
  {
  const int(102)
  ,
  {
  const int(103)
  ,
  const int(104)
  ,
  const int(105)
  }
  ,
  const int(106)
  }
  ;
  struct
  identifier(pair)
  identifier(pair1)
  =
  {
  const int(109)
  ,
  const int(110)
  }
  ;
  struct
  identifier(pair)
  identifier(pair2)
  =
  {
  const int(209)
  }
  ;
  struct
  identifier(outer)
  identifier(s3)
  =
  {
  const int(301)
  }
  ;
  struct
  identifier(outer)
  identifier(struct_array)
  [
  const int(4)
  ]
  =
  {
  identifier(s0)
  ,
  {
  const int(101)
  ,
  identifier(in1)
  ,
  string literal("107")
  ,
  const double(108.0)
  ,
  identifier(pair1)
  }
  ,
  {
  const int(201)
  ,
  {
  const int(202)
  ,
  {
  const int(203)
  }
  }
  ,
  string literal("207")
  ,
  const double(208.0)
  ,
  identifier(pair2)
  }
  ,
  identifier(s3)
  }
  ;
  return
  identifier(validate_array_of_structs)
  (
  identifier(struct_array)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_full_initialization)
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
  identifier(test_partial_initialization)
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
  identifier(test_mixed_initialization)
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
  identifier(test_array_of_structs)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
