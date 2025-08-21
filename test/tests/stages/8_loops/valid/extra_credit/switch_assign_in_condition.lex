-- Lexing ... OK
+
+
@@ Tokens @@
List[46]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(0)
  ;
  switch
  (
  identifier(a)
  =
  const int(1)
  )
  {
  case
  const int(0)
  :
  return
  const int(10)
  ;
  case
  const int(1)
  :
  identifier(a)
  =
  identifier(a)
  *
  const int(2)
  ;
  break
  ;
  default
  :
  identifier(a)
  =
  const int(99)
  ;
  }
  return
  identifier(a)
  ;
  }
