-- Lexing ... OK
+
+
@@ Tokens @@
List[201]:
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(long_arr)
  [
  const int(2)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  static
  int
  identifier(i)
  =
  const int(3)
  ;
  static
  unsigned
  char
  identifier(uc)
  =
  const int(4)
  ;
  double
  identifier(d)
  =
  const double(5.0)
  ;
  long
  *
  identifier(ptr)
  =
  identifier(long_arr)
  ;
  if
  (
  sizeof
  (
  identifier(long_arr)
  [
  const int(1)
  ]
  *=
  const int(10)
  )
  !=
  const int(8)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  sizeof
  (
  identifier(i)
  /=
  const unsigned long(10ul)
  )
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  (
  identifier(uc)
  %=
  const int(2)
  )
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  sizeof
  (
  identifier(d)
  -=
  const int(11)
  )
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  sizeof
  (
  identifier(ptr)
  +=
  const int(1)
  )
  !=
  const int(8)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(long_arr)
  [
  const int(0)
  ]
  !=
  const int(1)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(long_arr)
  [
  const int(1)
  ]
  !=
  const int(2)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(3)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(uc)
  !=
  const int(4)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(5.0)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(ptr)
  !=
  identifier(long_arr)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
