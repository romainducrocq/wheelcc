-- Lexing ... OK
+
+
@@ Tokens @@
List[158]:
  double
  identifier(flag)
  =
  const double(12e5)
  ;
  struct
  identifier(inner)
  {
  double
  identifier(a)
  ;
  double
  identifier(b)
  ;
  }
  ;
  struct
  identifier(s)
  {
  void
  *
  identifier(ptr)
  ;
  long
  identifier(arr)
  [
  const int(5)
  ]
  ;
  struct
  identifier(inner)
  identifier(x)
  ;
  char
  identifier(c)
  [
  const int(4)
  ]
  ;
  }
  ;
  long
  identifier(target)
  (
  void
  )
  {
  unsigned
  long
  identifier(x)
  =
  const int(4)
  ;
  char
  identifier(z)
  ;
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  &
  identifier(z)
  ,
  {
  const long(1l)
  ,
  const long(2l)
  ,
  }
  ,
  {
  const double(3.)
  ,
  const double(4.)
  }
  ,
  string literal("abc")
  }
  ;
  if
  (
  const int(4)
  -
  identifier(x)
  )
  {
  identifier(x)
  =
  identifier(my_struct)
  .
  identifier(c)
  [
  const int(2)
  ]
  ;
  identifier(z)
  =
  identifier(my_struct)
  .
  identifier(arr)
  [
  const int(1)
  ]
  ;
  identifier(my_struct)
  .
  identifier(x)
  .
  identifier(a)
  =
  identifier(z)
  *
  const double(100.)
  ;
  }
  if
  (
  !
  identifier(flag)
  )
  {
  identifier(z)
  =
  const int(10)
  +
  *
  (
  int
  *
  )
  identifier(my_struct)
  .
  identifier(ptr)
  ;
  }
  identifier(z)
  =
  identifier(x)
  +
  const int(5)
  ;
  return
  identifier(z)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
