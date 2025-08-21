-- Lexing ... OK
+
+
@@ Tokens @@
List[179]:
  static
  int
  long
  identifier(a)
  ;
  int
  static
  long
  identifier(a)
  ;
  long
  static
  identifier(a)
  ;
  int
  identifier(my_function)
  (
  long
  identifier(a)
  ,
  long
  int
  identifier(b)
  ,
  int
  long
  identifier(c)
  )
  ;
  int
  identifier(my_function)
  (
  long
  int
  identifier(x)
  ,
  int
  long
  identifier(y)
  ,
  long
  identifier(z)
  )
  {
  return
  identifier(x)
  +
  identifier(y)
  +
  identifier(z)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(x)
  =
  const long(1l)
  ;
  long
  int
  identifier(y)
  =
  const long(2l)
  ;
  int
  long
  identifier(z)
  =
  const long(3l)
  ;
  extern
  long
  identifier(a)
  ;
  identifier(a)
  =
  const int(4)
  ;
  int
  identifier(sum)
  =
  const int(0)
  ;
  for
  (
  long
  identifier(i)
  =
  const long(1099511627776l)
  ;
  identifier(i)
  >
  const int(0)
  ;
  identifier(i)
  =
  identifier(i)
  /
  const int(2)
  )
  {
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(y)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(a)
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(my_function)
  (
  identifier(x)
  ,
  identifier(y)
  ,
  identifier(z)
  )
  !=
  const int(6)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(sum)
  !=
  const int(41)
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
