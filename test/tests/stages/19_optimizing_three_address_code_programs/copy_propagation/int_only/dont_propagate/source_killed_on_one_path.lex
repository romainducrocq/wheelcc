-- Lexing ... OK
+
+
@@ Tokens @@
List[83]:
  int
  identifier(putchar)
  (
  int
  identifier(c)
  )
  ;
  int
  identifier(f)
  (
  int
  identifier(src)
  ,
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  identifier(src)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(src)
  =
  const int(65)
  ;
  }
  identifier(putchar)
  (
  identifier(src)
  )
  ;
  return
  identifier(x)
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
  identifier(f)
  (
  const int(68)
  ,
  const int(0)
  )
  !=
  const int(68)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(f)
  (
  const int(70)
  ,
  const int(1)
  )
  !=
  const int(70)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
