-- Lexing ... OK
+
+
@@ Tokens @@
List[67]:
  int
  identifier(update_x)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  identifier(update_x)
  (
  )
  ;
  extern
  int
  identifier(x)
  ;
  switch
  (
  identifier(x)
  )
  {
  case
  const int(0)
  :
  return
  const int(1)
  ;
  case
  const int(1)
  :
  return
  const int(2)
  ;
  case
  const int(4)
  :
  return
  const int(0)
  ;
  default
  :
  return
  const int(4)
  ;
  }
  }
  int
  identifier(x)
  ;
  int
  identifier(update_x)
  (
  void
  )
  {
  identifier(x)
  =
  const int(4)
  ;
  return
  const int(0)
  ;
  }
