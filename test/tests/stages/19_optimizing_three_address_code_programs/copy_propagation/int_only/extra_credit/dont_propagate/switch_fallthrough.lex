-- Lexing ... OK
+
+
@@ Tokens @@
List[51]:
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(retval)
  =
  const int(10)
  ;
  switch
  (
  identifier(flag)
  )
  {
  case
  const int(1)
  :
  identifier(retval)
  =
  const int(0)
  ;
  case
  const int(2)
  :
  return
  identifier(retval)
  ;
  default
  :
  return
  -
  const int(1)
  ;
  }
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  const int(1)
  )
  ;
  }
