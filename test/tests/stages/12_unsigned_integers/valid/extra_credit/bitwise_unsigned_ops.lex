-- Lexing ... OK
+
+
@@ Tokens @@
List[82]:
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
  -
  const unsigned int(1u)
  ;
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(9223372036854775808ul)
  ;
  if
  (
  (
  identifier(ui)
  &
  identifier(ul)
  )
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  if
  (
  (
  identifier(ui)
  |
  identifier(ul)
  )
  !=
  const unsigned long(9223372041149743103ul)
  )
  return
  const int(2)
  ;
  signed
  int
  identifier(i)
  =
  -
  const int(1)
  ;
  if
  (
  (
  identifier(i)
  &
  identifier(ul)
  )
  !=
  identifier(ul)
  )
  return
  const int(3)
  ;
  if
  (
  (
  identifier(i)
  |
  identifier(ul)
  )
  !=
  identifier(i)
  )
  return
  const int(4)
  ;
  return
  const int(0)
  ;
  }
