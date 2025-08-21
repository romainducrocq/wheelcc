-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(switch_on_long)
  (
  long
  identifier(l)
  )
  {
  switch
  (
  identifier(l)
  )
  {
  case
  const int(0)
  :
  return
  const int(0)
  ;
  case
  const int(100)
  :
  return
  const int(1)
  ;
  case
  const long(8589934592l)
  :
  return
  const int(2)
  ;
  default
  :
  return
  -
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
  if
  (
  identifier(switch_on_long)
  (
  const int(8589934592)
  )
  !=
  const int(2)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(switch_on_long)
  (
  const int(100)
  )
  !=
  const int(1)
  )
  return
  const int(2)
  ;
  return
  const int(0)
  ;
  }
