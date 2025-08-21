-- Lexing ... OK
+
+
@@ Tokens @@
List[98]:
  int
  identifier(i)
  =
  const int(0)
  ;
  int
  identifier(target)
  (
  int
  identifier(arg)
  )
  {
  identifier(i)
  =
  const int(5)
  ;
  identifier(i)
  =
  identifier(arg)
  ;
  return
  identifier(i)
  +
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(result1)
  =
  identifier(target)
  (
  const int(2)
  )
  ;
  if
  (
  identifier(i)
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
  identifier(result1)
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(result2)
  =
  identifier(target)
  (
  -
  const int(1)
  )
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(result2)
  !=
  const int(0)
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
