-- Lexing ... OK
+
+
@@ Tokens @@
List[98]:
  int
  identifier(globvar)
  =
  const int(0)
  ;
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
  identifier(globvar)
  =
  const int(1)
  ;
  case
  const int(2)
  :
  identifier(globvar)
  =
  identifier(globvar)
  +
  const int(3)
  ;
  case
  const int(3)
  :
  identifier(globvar)
  =
  identifier(globvar)
  *
  const int(2)
  ;
  default
  :
  identifier(retval)
  =
  const int(3)
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
  int
  identifier(retval)
  =
  identifier(target)
  (
  const int(2)
  )
  ;
  if
  (
  identifier(retval)
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
  identifier(globvar)
  !=
  const int(6)
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
