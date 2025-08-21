-- Lexing ... OK
+
+
@@ Tokens @@
List[96]:
  int
  identifier(switch_on_int)
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
  const int(5)
  :
  return
  const int(0)
  ;
  case
  const long(8589934592l)
  :
  return
  const int(1)
  ;
  case
  const int(34359738367)
  :
  return
  const int(2)
  ;
  default
  :
  return
  const int(3)
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
  identifier(switch_on_int)
  (
  const int(5)
  )
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(switch_on_int)
  (
  const int(0)
  )
  !=
  const int(1)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(switch_on_int)
  (
  -
  const int(1)
  )
  !=
  const int(2)
  )
  return
  const int(3)
  ;
  if
  (
  identifier(switch_on_int)
  (
  const int(17179869184)
  )
  !=
  const int(1)
  )
  return
  const int(4)
  ;
  return
  const int(0)
  ;
  }
