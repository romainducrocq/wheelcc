-- Lexing ... OK
+
+
@@ Tokens @@
List[71]:
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  const int(0)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(x)
  =
  const int(3)
  ;
  }
  else
  {
  identifier(x)
  =
  const int(3)
  ;
  }
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
  identifier(target)
  (
  const int(1)
  )
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
  identifier(target)
  (
  const int(0)
  )
  !=
  const int(3)
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
