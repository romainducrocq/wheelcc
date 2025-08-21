-- Lexing ... OK
+
+
@@ Tokens @@
List[186]:
  union
  identifier(u)
  {
  unsigned
  long
  identifier(l)
  ;
  double
  identifier(d)
  ;
  char
  identifier(arr)
  [
  const int(8)
  ]
  ;
  }
  ;
  static
  union
  identifier(u)
  identifier(my_union)
  =
  {
  const unsigned long(18446744073709551615UL)
  }
  ;
  static
  union
  identifier(u)
  *
  identifier(union_ptr)
  =
  const int(0)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(union_ptr)
  =
  &
  identifier(my_union)
  ;
  if
  (
  identifier(my_union)
  .
  identifier(l)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(1)
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
  const int(8)
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
  identifier(my_union)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  -
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  }
  identifier(union_ptr)
  ->
  identifier(d)
  =
  -
  const double(1.0)
  ;
  if
  (
  identifier(union_ptr)
  ->
  identifier(l)
  !=
  const unsigned long(13830554455654793216ul)
  )
  {
  return
  const int(3)
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
  const int(6)
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
  identifier(my_union)
  .
  identifier(arr)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(4)
  ;
  }
  }
  if
  (
  identifier(union_ptr)
  ->
  identifier(arr)
  [
  const int(6)
  ]
  !=
  -
  const int(16)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(union_ptr)
  ->
  identifier(arr)
  [
  const int(7)
  ]
  !=
  -
  const int(65)
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
