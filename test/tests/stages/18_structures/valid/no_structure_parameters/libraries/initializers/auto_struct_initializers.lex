-- Lexing ... OK
+
+
@@ Tokens @@
List[428]:
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
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  void
  *
  identifier(calloc)
  (
  unsigned
  long
  identifier(nmemb)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  struct
  identifier(s)
  {
  char
  *
  identifier(one_msg)
  ;
  unsigned
  char
  identifier(two_arr)
  [
  const int(3)
  ]
  ;
  struct
  identifier(s)
  *
  identifier(three_self_ptr)
  ;
  double
  identifier(four_d)
  ;
  double
  *
  identifier(five_d_ptr)
  ;
  }
  ;
  int
  identifier(validate_full_initialization)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(validate_partial_initialization)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  ,
  char
  *
  identifier(expected_msg)
  )
  ;
  int
  identifier(validate_converted)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  )
  ;
  int
  identifier(validate_two_structs)
  (
  struct
  identifier(s)
  *
  identifier(ptr1)
  ,
  struct
  identifier(s)
  *
  identifier(ptr2)
  )
  ;
  int
  identifier(validate_full_initialization)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  )
  {
  if
  (
  identifier(strcmp)
  (
  identifier(ptr)
  ->
  identifier(one_msg)
  ,
  string literal("I'm a struct!")
  )
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const char('s')
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const char('u')
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const char('p')
  ||
  identifier(ptr)
  ->
  identifier(three_self_ptr)
  !=
  identifier(ptr)
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  !=
  const double(2e12)
  ||
  *
  identifier(ptr)
  ->
  identifier(five_d_ptr)
  !=
  const double(2e12)
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
  identifier(s)
  *
  identifier(ptr)
  ,
  char
  *
  identifier(expected_msg)
  )
  {
  if
  (
  identifier(ptr)
  ->
  identifier(one_msg)
  !=
  identifier(expected_msg)
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const char('a')
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const char('b')
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
  identifier(three_self_ptr)
  ->
  identifier(one_msg)
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
  identifier(two_arr)
  [
  const int(2)
  ]
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  ||
  identifier(ptr)
  ->
  identifier(five_d_ptr)
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
  identifier(validate_converted)
  (
  struct
  identifier(s)
  *
  identifier(ptr)
  )
  {
  if
  (
  !
  identifier(ptr)
  ->
  identifier(one_msg)
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const int(220)
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const int(232)
  ||
  identifier(ptr)
  ->
  identifier(two_arr)
  [
  const int(2)
  ]
  !=
  const int(224)
  ||
  identifier(ptr)
  ->
  identifier(three_self_ptr)
  ||
  identifier(ptr)
  ->
  identifier(four_d)
  !=
  const double(2999.0)
  ||
  *
  identifier(ptr)
  ->
  identifier(five_d_ptr)
  !=
  const double(0.0)
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
  identifier(validate_two_structs)
  (
  struct
  identifier(s)
  *
  identifier(ptr1)
  ,
  struct
  identifier(s)
  *
  identifier(ptr2)
  )
  {
  if
  (
  identifier(strcmp)
  (
  identifier(ptr2)
  ->
  identifier(one_msg)
  ,
  string literal("Yet another string")
  )
  ||
  identifier(ptr2)
  ->
  identifier(one_msg)
  !=
  identifier(ptr1)
  ->
  identifier(one_msg)
  ||
  identifier(ptr2)
  ->
  identifier(two_arr)
  [
  const int(0)
  ]
  !=
  const char('x')
  ||
  identifier(ptr2)
  ->
  identifier(two_arr)
  [
  const int(1)
  ]
  !=
  const char('y')
  ||
  identifier(ptr2)
  ->
  identifier(three_self_ptr)
  !=
  identifier(ptr1)
  ||
  identifier(ptr2)
  ->
  identifier(four_d)
  !=
  const double(150.0)
  ||
  *
  identifier(ptr1)
  ->
  identifier(five_d_ptr)
  !=
  const double(123.4)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(ptr1)
  ->
  identifier(two_arr)
  ==
  identifier(ptr2)
  ->
  identifier(two_arr)
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
