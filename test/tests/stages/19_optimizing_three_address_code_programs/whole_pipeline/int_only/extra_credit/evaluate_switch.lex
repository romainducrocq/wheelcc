-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
  int
  identifier(callee)
  (
  void
  )
  {
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
  identifier(switch_var)
  =
  const int(10)
  ;
  int
  identifier(retval)
  =
  -
  const int(1)
  ;
  switch
  (
  identifier(switch_var)
  )
  {
  case
  const int(1)
  :
  identifier(callee)
  (
  )
  ;
  return
  const int(1)
  ;
  case
  const int(2)
  :
  identifier(retval)
  =
  -
  const int(2)
  ;
  break
  ;
  case
  const int(10)
  :
  identifier(retval)
  =
  const int(0)
  ;
  break
  ;
  default
  :
  identifier(retval)
  =
  const int(1000)
  ;
  break
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
  )
  ;
  }
