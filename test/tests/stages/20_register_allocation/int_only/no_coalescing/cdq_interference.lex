-- Lexing ... OK
+
+
@@ Tokens @@
List[64]:
  int
  identifier(target)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  )
  {
  static
  int
  identifier(i)
  =
  const int(100)
  ;
  if
  (
  identifier(a)
  ||
  identifier(b)
  )
  {
  return
  const int(0)
  ;
  }
  return
  identifier(i)
  /
  identifier(c)
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
  identifier(target)
  (
  const int(0)
  ,
  const int(0)
  ,
  const int(10)
  )
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
