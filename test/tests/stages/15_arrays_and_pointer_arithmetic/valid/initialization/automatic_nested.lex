-- Lexing ... OK
+
+
@@ Tokens @@
List[585]:
  int
  identifier(test_simple)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  [
  const int(3)
  ]
  =
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ,
  {
  const int(4)
  ,
  const int(5)
  ,
  const int(6)
  }
  ,
  {
  const int(7)
  ,
  const int(8)
  ,
  const int(9)
  }
  }
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(3)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(3)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  if
  (
  identifier(arr)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  !=
  identifier(i)
  *
  const int(3)
  +
  identifier(j)
  +
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  }
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_partial)
  (
  void
  )
  {
  int
  identifier(first_half_only)
  [
  const int(4)
  ]
  [
  const int(2)
  ]
  [
  const int(6)
  ]
  =
  {
  {
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  }
  ,
  {
  {
  const int(4)
  ,
  const int(5)
  ,
  const int(6)
  }
  }
  }
  ;
  int
  identifier(expected)
  =
  const int(1)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(4)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(j)
  =
  const int(0)
  ;
  identifier(j)
  <
  const int(2)
  ;
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  )
  {
  for
  (
  int
  identifier(k)
  =
  const int(0)
  ;
  identifier(k)
  <
  const int(6)
  ;
  identifier(k)
  =
  identifier(k)
  +
  const int(1)
  )
  {
  int
  identifier(val)
  =
  identifier(first_half_only)
  [
  identifier(i)
  ]
  [
  identifier(j)
  ]
  [
  identifier(k)
  ]
  ;
  if
  (
  identifier(i)
  >
  const int(1)
  ||
  identifier(j)
  >
  const int(0)
  ||
  identifier(k)
  >
  const int(2)
  )
  {
  if
  (
  identifier(val)
  )
  {
  return
  const int(0)
  ;
  }
  }
  else
  {
  if
  (
  identifier(val)
  !=
  identifier(expected)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(expected)
  =
  identifier(expected)
  +
  const int(1)
  ;
  }
  }
  }
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_non_constant_and_type_conversion)
  (
  void
  )
  {
  extern
  unsigned
  int
  identifier(three)
  (
  void
  )
  ;
  static
  int
  identifier(x)
  =
  const int(2000)
  ;
  int
  identifier(negative_four)
  =
  -
  const int(4)
  ;
  int
  *
  identifier(ptr)
  =
  &
  identifier(negative_four)
  ;
  double
  identifier(arr)
  [
  const int(3)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  identifier(x)
  ,
  identifier(x)
  /
  *
  identifier(ptr)
  }
  ,
  {
  identifier(three)
  (
  )
  }
  ,
  }
  ;
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const double(2000.0)
  ||
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(1)
  ]
  !=
  -
  const double(500.0)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  !=
  const double(3.0)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(1)
  ]
  [
  const int(1)
  ]
  ||
  identifier(arr)
  [
  const int(2)
  ]
  [
  const int(0)
  ]
  ||
  identifier(arr)
  [
  const int(2)
  ]
  [
  const int(1)
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
  unsigned
  int
  identifier(three)
  (
  void
  )
  {
  return
  const unsigned int(3u)
  ;
  }
  long
  identifier(one)
  =
  const long(1l)
  ;
  int
  identifier(test_preserve_stack)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(1)
  ;
  int
  identifier(arr)
  [
  const int(3)
  ]
  [
  const int(1)
  ]
  =
  {
  {
  identifier(one)
  *
  const long(2l)
  }
  ,
  {
  identifier(one)
  +
  identifier(three)
  (
  )
  }
  }
  ;
  unsigned
  int
  identifier(u)
  =
  const int(2684366905)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(u)
  !=
  const int(2684366905)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  [
  const int(0)
  ]
  !=
  const int(2)
  ||
  identifier(arr)
  [
  const int(1)
  ]
  [
  const int(0)
  ]
  !=
  const int(4)
  ||
  identifier(arr)
  [
  const int(2)
  ]
  [
  const int(0)
  ]
  !=
  const int(0)
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
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_simple)
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
  identifier(test_partial)
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
  identifier(test_non_constant_and_type_conversion)
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
  identifier(test_preserve_stack)
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
