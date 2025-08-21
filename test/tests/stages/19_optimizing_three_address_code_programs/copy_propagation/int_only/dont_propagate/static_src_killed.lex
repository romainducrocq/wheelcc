-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(x)
  =
  const int(1)
  ;
  int
  identifier(f)
  (
  void
  )
  {
  identifier(x)
  =
  const int(4)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(y)
  =
  identifier(x)
  ;
  identifier(f)
  (
  )
  ;
  return
  identifier(y)
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
  )
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
  identifier(x)
  !=
  const int(4)
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
