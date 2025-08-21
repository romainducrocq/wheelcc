-- Lexing ... OK
+
+
@@ Tokens @@
List[81]:
  int
  identifier(putchar)
  (
  int
  identifier(c)
  )
  ;
  int
  identifier(fib)
  (
  int
  identifier(count)
  )
  {
  int
  identifier(n0)
  =
  const int(0)
  ;
  int
  identifier(n1)
  =
  const int(1)
  ;
  int
  identifier(i)
  =
  const int(0)
  ;
  do
  {
  int
  identifier(n2)
  =
  identifier(n0)
  +
  identifier(n1)
  ;
  identifier(n0)
  =
  identifier(n1)
  ;
  identifier(n1)
  =
  identifier(n2)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  }
  while
  (
  identifier(i)
  <
  identifier(count)
  )
  ;
  return
  identifier(n1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  (
  identifier(fib)
  (
  const int(20)
  )
  ==
  const int(10946)
  )
  ;
  }
