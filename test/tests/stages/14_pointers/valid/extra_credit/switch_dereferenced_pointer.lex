-- Lexing ... OK
+
+
@@ Tokens @@
List[62]:
  long
  identifier(l)
  =
  const long(4294967300l)
  ;
  long
  *
  identifier(get_ptr)
  (
  void
  )
  {
  return
  &
  identifier(l)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  switch
  (
  *
  identifier(get_ptr)
  (
  )
  )
  {
  case
  const int(1)
  :
  return
  const int(1)
  ;
  case
  const int(4)
  :
  return
  const int(2)
  ;
  case
  const long(4294967300l)
  :
  return
  const int(0)
  ;
  case
  const unsigned long(18446744073709551600UL)
  :
  return
  const int(3)
  ;
  default
  :
  return
  const int(4)
  ;
  }
  }
