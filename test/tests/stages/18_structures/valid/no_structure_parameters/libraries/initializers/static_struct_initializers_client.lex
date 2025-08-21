-- Lexing ... OK
+
+
@@ Tokens @@
List[180]:
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
  struct
  identifier(s)
  identifier(uninitialized)
  ;
  struct
  identifier(s)
  identifier(partial)
  =
  {
  const double(1.0)
  ,
  string literal("Hello")
  }
  ;
  struct
  identifier(s)
  identifier(partial_with_array)
  =
  {
  const double(3.0)
  ,
  string literal("!")
  ,
  {
  const int(1)
  }
  ,
  const int(2)
  }
  ;
  struct
  identifier(s)
  identifier(converted)
  =
  {
  const long(1152921504606846977l)
  ,
  const long(0l)
  ,
  string literal("abc")
  ,
  const long(17179869189l)
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
  identifier(test_partial_inner_init)
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
  identifier(test_implicit_conversion)
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
