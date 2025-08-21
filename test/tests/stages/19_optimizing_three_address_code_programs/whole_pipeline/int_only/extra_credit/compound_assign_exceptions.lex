-- Lexing ... OK
+
+
@@ Tokens @@
List[102]:
  static
  int
  identifier(zero)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(w)
  =
  const int(3)
  ;
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  identifier(y)
  =
  const int(2147483647)
  ;
  int
  identifier(z)
  =
  -
  const int(2147483647)
  ;
  if
  (
  identifier(zero)
  )
  {
  identifier(w)
  %=
  const int(0)
  ;
  identifier(x)
  /=
  const int(0)
  ;
  identifier(y)
  +=
  const int(10)
  ;
  identifier(z)
  -=
  const int(10)
  ;
  }
  if
  (
  identifier(w)
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
  identifier(x)
  !=
  const int(10)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(y)
  !=
  const int(2147483647)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(z)
  !=
  -
  const int(2147483647)
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
