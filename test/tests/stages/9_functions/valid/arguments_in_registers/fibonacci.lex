-- Lexing ... OK
+
+
@@ Tokens @@
List[59]:
  int
  identifier(fib)
  (
  int
  identifier(n)
  )
  {
  if
  (
  identifier(n)
  ==
  const int(0)
  ||
  identifier(n)
  ==
  const int(1)
  )
  {
  return
  identifier(n)
  ;
  }
  else
  {
  return
  identifier(fib)
  (
  identifier(n)
  -
  const int(1)
  )
  +
  identifier(fib)
  (
  identifier(n)
  -
  const int(2)
  )
  ;
  }
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(n)
  =
  const int(6)
  ;
  return
  identifier(fib)
  (
  identifier(n)
  )
  ;
  }
