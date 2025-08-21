-- Lexing ... OK
+
+
@@ Tokens @@
List[116]:
  int
  identifier(fib)
  (
  int
  identifier(a)
  )
  ;
  int
  identifier(multiply_many_args)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  int
  identifier(f)
  ,
  int
  identifier(g)
  ,
  int
  identifier(h)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  identifier(fib)
  (
  const int(4)
  )
  ;
  int
  identifier(seven)
  =
  const int(7)
  ;
  int
  identifier(eight)
  =
  identifier(fib)
  (
  const int(6)
  )
  ;
  int
  identifier(y)
  =
  identifier(multiply_many_args)
  (
  identifier(x)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  ,
  const int(6)
  ,
  identifier(seven)
  ,
  identifier(eight)
  )
  ;
  if
  (
  identifier(x)
  !=
  const int(3)
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
  const int(589680)
  )
  {
  return
  const int(2)
  ;
  }
  return
  identifier(x)
  +
  (
  identifier(y)
  %
  const int(256)
  )
  ;
  }
