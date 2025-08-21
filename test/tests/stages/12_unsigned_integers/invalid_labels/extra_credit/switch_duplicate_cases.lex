-- Lexing ... OK
+
+
@@ Tokens @@
List[36]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(10u)
  ;
  switch
  (
  identifier(ui)
  )
  {
  case
  const unsigned int(4294967295u)
  :
  return
  const int(0)
  ;
  case
  const long(1099511627775l)
  :
  return
  const int(1)
  ;
  default
  :
  return
  const int(2)
  ;
  }
  }
