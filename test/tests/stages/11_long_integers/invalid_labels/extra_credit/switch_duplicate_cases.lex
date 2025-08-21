-- Lexing ... OK
+
+
@@ Tokens @@
List[44]:
  int
  identifier(switch_statement)
  (
  int
  identifier(i)
  )
  {
  switch
  (
  identifier(i)
  )
  {
  case
  const int(0)
  :
  return
  const int(0)
  ;
  case
  const int(17179869184)
  :
  return
  const int(0)
  ;
  default
  :
  return
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
  identifier(switch_statement)
  (
  const int(0)
  )
  ;
  }
