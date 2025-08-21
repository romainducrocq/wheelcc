-- Lexing ... OK
+
+
@@ Tokens @@
List[70]:
  int
  identifier(f)
  (
  int
  identifier(arg)
  ,
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  identifier(arg)
  *
  const int(2)
  ;
  if
  (
  identifier(flag)
  )
  return
  identifier(x)
  ;
  return
  const int(0)
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
  const int(20)
  ,
  const int(1)
  )
  !=
  const int(40)
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
  const int(3)
  ,
  const int(0)
  )
  !=
  const int(0)
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
