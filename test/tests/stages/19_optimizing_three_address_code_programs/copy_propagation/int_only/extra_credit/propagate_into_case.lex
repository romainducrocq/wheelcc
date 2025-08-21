-- Lexing ... OK
+
+
@@ Tokens @@
List[94]:
  int
  identifier(globvar)
  =
  const int(0)
  ;
  int
  identifier(callee)
  (
  int
  identifier(arg)
  )
  {
  identifier(globvar)
  =
  identifier(arg)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(arg)
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
  identifier(arg)
  =
  const int(20)
  ;
  break
  ;
  case
  const int(2)
  :
  identifier(callee)
  (
  identifier(arg)
  )
  ;
  break
  ;
  default
  :
  identifier(globvar)
  =
  -
  const int(1)
  ;
  }
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
  identifier(target)
  (
  const int(2)
  )
  ;
  if
  (
  identifier(globvar)
  ==
  const int(10)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
