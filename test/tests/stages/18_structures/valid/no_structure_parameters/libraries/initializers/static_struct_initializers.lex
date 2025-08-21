-- Lexing ... OK
+
+
@@ Tokens @@
List[320]:
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
  identifier(s)
  {
  double
  identifier(one_d)
  ;
  char
  *
  identifier(two_msg)
  ;
  unsigned
  char
  identifier(three_arr)
  [
  const int(3)
  ]
  ;
  int
  identifier(four_i)
  ;
  }
  ;
  extern
  struct
  identifier(s)
  identifier(uninitialized)
  ;
  extern
  struct
  identifier(s)
  identifier(partial)
  ;
  extern
  struct
  identifier(s)
  identifier(partial_with_array)
  ;
  extern
  struct
  identifier(s)
  identifier(converted)
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
  identifier(test_partial_inner_init)
  (
  void
  )
  ;
  int
  identifier(test_implicit_conversion)
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
  identifier(uninitialized)
  .
  identifier(one_d)
  ||
  identifier(uninitialized)
  .
  identifier(two_msg)
  ||
  identifier(uninitialized)
  .
  identifier(three_arr)
  [
  const int(0)
  ]
  ||
  identifier(uninitialized)
  .
  identifier(three_arr)
  [
  const int(1)
  ]
  ||
  identifier(uninitialized)
  .
  identifier(three_arr)
  [
  const int(2)
  ]
  ||
  identifier(uninitialized)
  .
  identifier(four_i)
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
  identifier(one_d)
  !=
  const double(1.0)
  ||
  identifier(strcmp)
  (
  identifier(partial)
  .
  identifier(two_msg)
  ,
  string literal("Hello")
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
  identifier(three_arr)
  [
  const int(0)
  ]
  ||
  identifier(partial)
  .
  identifier(three_arr)
  [
  const int(1)
  ]
  ||
  identifier(partial)
  .
  identifier(three_arr)
  [
  const int(2)
  ]
  ||
  identifier(partial)
  .
  identifier(four_i)
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
  identifier(test_partial_inner_init)
  (
  void
  )
  {
  if
  (
  identifier(partial_with_array)
  .
  identifier(one_d)
  !=
  const double(3.0)
  ||
  identifier(strcmp)
  (
  identifier(partial_with_array)
  .
  identifier(two_msg)
  ,
  string literal("!")
  )
  ||
  identifier(partial_with_array)
  .
  identifier(three_arr)
  [
  const int(0)
  ]
  !=
  const int(1)
  ||
  identifier(partial_with_array)
  .
  identifier(four_i)
  !=
  const int(2)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(partial_with_array)
  .
  identifier(three_arr)
  [
  const int(1)
  ]
  ||
  identifier(partial_with_array)
  .
  identifier(three_arr)
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
  identifier(test_implicit_conversion)
  (
  void
  )
  {
  if
  (
  identifier(converted)
  .
  identifier(one_d)
  !=
  const double(1152921504606846976.0)
  ||
  identifier(converted)
  .
  identifier(two_msg)
  ||
  identifier(converted)
  .
  identifier(three_arr)
  [
  const int(0)
  ]
  !=
  const char('a')
  ||
  identifier(converted)
  .
  identifier(three_arr)
  [
  const int(1)
  ]
  !=
  const char('b')
  ||
  identifier(converted)
  .
  identifier(three_arr)
  [
  const int(2)
  ]
  !=
  const char('c')
  ||
  identifier(converted)
  .
  identifier(four_i)
  !=
  const int(5)
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
