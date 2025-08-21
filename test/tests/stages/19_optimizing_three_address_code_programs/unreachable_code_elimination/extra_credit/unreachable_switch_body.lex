-- Lexing ... OK
+
+
@@ Tokens @@
List[90]:
  int
  identifier(target)
  (
  int
  identifier(flag)
  )
  {
  switch
  (
  identifier(flag)
  )
  {
  static
  int
  identifier(x)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  identifier(flag)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  switch
  (
  identifier(i)
  )
  {
  case
  const int(1)
  :
  identifier(x)
  =
  identifier(x)
  +
  const int(1)
  ;
  case
  const int(2)
  :
  identifier(x)
  =
  identifier(x)
  +
  const int(2)
  ;
  default
  :
  identifier(x)
  =
  identifier(x)
  *
  const int(3)
  ;
  }
  }
  return
  identifier(x)
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
  return
  identifier(target)
  (
  const int(20)
  )
  ;
  }
