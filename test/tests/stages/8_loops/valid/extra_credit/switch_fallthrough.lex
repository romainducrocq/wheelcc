-- Lexing ... OK
+
+
@@ Tokens @@
List[64]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(4)
  ;
  int
  identifier(b)
  =
  const int(9)
  ;
  int
  identifier(c)
  =
  const int(0)
  ;
  switch
  (
  identifier(a)
  ?
  identifier(b)
  :
  const int(7)
  )
  {
  case
  const int(0)
  :
  return
  const int(5)
  ;
  case
  const int(7)
  :
  identifier(c)
  =
  const int(1)
  ;
  case
  const int(9)
  :
  identifier(c)
  =
  const int(2)
  ;
  case
  const int(1)
  :
  identifier(c)
  =
  identifier(c)
  +
  const int(4)
  ;
  }
  return
  identifier(c)
  ;
  }
