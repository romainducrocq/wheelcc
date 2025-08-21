-- Lexing ... OK
+
+
@@ Tokens @@
List[47]:
  int
  identifier(switch_statement)
  (
  int
  identifier(i)
  )
  {
  switch
  (
  (
  long
  )
  identifier(i)
  )
  {
  case
  const long(100l)
  :
  return
  const int(0)
  ;
  case
  const int(100)
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
  const int(100)
  )
  ;
  }
