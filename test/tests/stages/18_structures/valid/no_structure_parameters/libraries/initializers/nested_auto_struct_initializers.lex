-- Lexing ... OK
+
+
@@ Tokens @@
List[893]:
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
  identifier(validate_full_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  {
  if
  (
  identifier(ptr)
  ->
  identifier(one_l)
  !=
  -
  const long(200l)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  -
  const int(171)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(200)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const int(202)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const int(203)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const unsigned int(40u)
  ||
  identifier(strcmp)
  (
  identifier(ptr)
  ->
  identifier(three_msg)
  ,
  string literal("Important message!")
  )
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  !=
  -
  const double(22.)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(a)
  !=
  const int(1)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(b)
  !=
  const int(2)
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
  identifier(validate_partial_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  {
  if
  (
  identifier(ptr)
  ->
  identifier(one_l)
  !=
  const int(1000)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(1)
  ||
  identifier(strcmp)
  (
  identifier(ptr)
  ->
  identifier(three_msg)
  ,
  string literal("Partial")
  )
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(three_u)
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(a)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(b)
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
  identifier(validate_mixed_initialization)
  (
  struct
  identifier(outer)
  *
  identifier(ptr)
  )
  {
  if
  (
  identifier(ptr)
  ->
  identifier(one_l)
  !=
  const int(200)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(20)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(21)
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(three_u)
  !=
  const unsigned int(22u)
  ||
  identifier(strcmp)
  (
  identifier(ptr)
  ->
  identifier(three_msg)
  ,
  string literal("mixed")
  )
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  !=
  const double(10.0)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(a)
  !=
  const int(99)
  ||
  identifier(ptr)
  ->
  identifier(five_pair)
  .
  identifier(b)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  ||
  identifier(ptr)
  ->
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
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
  identifier(validate_array_of_structs)
  (
  struct
  identifier(outer)
  *
  identifier(struct_array)
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
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(2)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(3)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const int(4)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const int(5)
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
  const int(6)
  ||
  identifier(strcmp)
  (
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(three_msg)
  ,
  string literal("7")
  )
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(four_d)
  !=
  const double(8.0)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(five_pair)
  .
  identifier(a)
  !=
  const int(9)
  ||
  identifier(struct_array)
  [
  const int(0)
  ]
  .
  identifier(five_pair)
  .
  identifier(b)
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
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(one_l)
  !=
  const int(101)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(102)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(103)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const int(104)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(two_struct)
  .
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const int(105)
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
  const int(106)
  ||
  identifier(strcmp)
  (
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(three_msg)
  ,
  string literal("107")
  )
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(four_d)
  !=
  const double(108.0)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(five_pair)
  .
  identifier(a)
  !=
  const int(109)
  ||
  identifier(struct_array)
  [
  const int(1)
  ]
  .
  identifier(five_pair)
  .
  identifier(b)
  !=
  const int(110)
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
  !=
  const int(201)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  !=
  const int(202)
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
  !=
  const int(203)
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
  ||
  identifier(strcmp)
  (
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(three_msg)
  ,
  string literal("207")
  )
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(four_d)
  !=
  const double(208.0)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(five_pair)
  .
  identifier(a)
  !=
  const int(209)
  ||
  identifier(struct_array)
  [
  const int(2)
  ]
  .
  identifier(five_pair)
  .
  identifier(b)
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
  const int(3)
  ]
  .
  identifier(one_l)
  !=
  const int(301)
  ||
  identifier(struct_array)
  [
  const int(3)
  ]
  .
  identifier(two_struct)
  .
  identifier(one_i)
  ||
  identifier(struct_array)
  [
  const int(3)
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
  const int(3)
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
  const int(3)
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
  const int(3)
  ]
  .
  identifier(two_struct)
  .
  identifier(three_u)
  ||
  identifier(struct_array)
  [
  const int(3)
  ]
  .
  identifier(three_msg)
  ||
  identifier(struct_array)
  [
  const int(3)
  ]
  .
  identifier(four_d)
  ||
  identifier(struct_array)
  [
  const int(3)
  ]
  .
  identifier(five_pair)
  .
  identifier(a)
  ||
  identifier(struct_array)
  [
  const int(3)
  ]
  .
  identifier(five_pair)
  .
  identifier(b)
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
