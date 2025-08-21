-- Lexing ... OK
+
+
@@ Tokens @@
List[58]:
  int
  identifier(x)
  =
  const int(0)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  identifier(x)
  =
  const int(10)
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
  int
  identifier(result)
  =
  identifier(target)
  (
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(10)
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
  return
  const int(0)
  ;
  }
