-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
  int
  identifier(target)
  (
  int
  identifier(x)
  ,
  int
  identifier(y)
  )
  {
  if
  (
  identifier(x)
  )
  {
  if
  (
  identifier(y)
  )
  {
  }
  }
  return
  const int(1)
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
  const int(1)
  ,
  const int(1)
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
  identifier(target)
  (
  const int(0)
  ,
  const int(0)
  )
  !=
  const int(1)
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
