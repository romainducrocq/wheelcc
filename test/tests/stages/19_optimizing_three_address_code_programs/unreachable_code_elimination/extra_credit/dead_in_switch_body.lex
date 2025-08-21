-- Lexing ... OK
+
+
@@ Tokens @@
List[87]:
  int
  identifier(callee)
  (
  void
  )
  {
  return
  -
  const int(1)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(x)
  )
  {
  int
  identifier(retval)
  =
  const int(0)
  ;
  switch
  (
  identifier(x)
  )
  {
  case
  const int(1)
  :
  identifier(retval)
  =
  const int(1)
  ;
  break
  ;
  case
  const int(2)
  :
  identifier(retval)
  =
  const int(2)
  ;
  break
  ;
  identifier(callee)
  (
  )
  ;
  case
  const int(3)
  :
  identifier(retval)
  =
  const int(10)
  ;
  break
  ;
  default
  :
  return
  -
  const int(1)
  ;
  identifier(callee)
  (
  )
  ;
  }
  return
  identifier(retval)
  ;
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
  const int(3)
  )
  ;
  }
