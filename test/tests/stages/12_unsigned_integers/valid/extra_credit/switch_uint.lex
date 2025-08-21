-- Lexing ... OK
+
+
@@ Tokens @@
List[84]:
  int
  identifier(switch_on_uint)
  (
  unsigned
  int
  identifier(ui)
  )
  {
  switch
  (
  identifier(ui)
  )
  {
  case
  const unsigned int(5u)
  :
  return
  const int(0)
  ;
  case
  const long(4294967286l)
  :
  return
  const int(1)
  ;
  case
  const unsigned long(34359738378ul)
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
  identifier(switch_on_uint)
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
  identifier(switch_on_uint)
  (
  const int(4294967286)
  )
  !=
  const int(1)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(switch_on_uint)
  (
  const int(10)
  )
  !=
  const int(2)
  )
  return
  const int(3)
  ;
  return
  const int(0)
  ;
  }
