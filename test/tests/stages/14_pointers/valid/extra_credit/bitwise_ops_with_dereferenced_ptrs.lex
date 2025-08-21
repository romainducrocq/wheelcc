-- Lexing ... OK
+
+
@@ Tokens @@
List[121]:
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
  unsigned
  int
  *
  identifier(ui_ptr)
  =
  &
  identifier(ui)
  ;
  unsigned
  long
  *
  identifier(ul_ptr)
  =
  &
  identifier(ul)
  ;
  if
  (
  (
  *
  identifier(ui_ptr)
  &
  *
  identifier(ul_ptr)
  )
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  *
  identifier(ui_ptr)
  |
  *
  identifier(ul_ptr)
  )
  !=
  const unsigned long(9223372041149743103ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  -
  const int(1)
  ;
  signed
  int
  *
  identifier(i_ptr)
  =
  &
  identifier(i)
  ;
  if
  (
  (
  *
  identifier(i_ptr)
  &
  identifier(ul)
  )
  !=
  *
  identifier(ul_ptr)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  *
  identifier(i_ptr)
  |
  *
  identifier(ul_ptr)
  )
  !=
  identifier(i)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
