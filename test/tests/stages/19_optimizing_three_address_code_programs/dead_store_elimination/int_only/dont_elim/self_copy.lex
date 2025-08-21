-- Lexing ... OK
+
+
@@ Tokens @@
List[68]:
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(i)
  =
  const int(2)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(i)
  =
  const int(3)
  ;
  }
  identifier(i)
  =
  identifier(i)
  ;
  return
  identifier(i)
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
  )
  !=
  const int(2)
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
  const int(1)
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
