-- Lexing ... OK
+
+
@@ Tokens @@
List[134]:
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446460386757245432ul)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  long
  *
  identifier(ul_ptr)
  =
  &
  identifier(ul)
  ;
  *
  identifier(ul_ptr)
  &=
  -
  const int(1000)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446460386757244952ul)
  )
  {
  return
  const int(1)
  ;
  }
  *
  identifier(ul_ptr)
  |=
  const unsigned int(4294967040u)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446460386824683288ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  const int(123456)
  ;
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(4042322160u)
  ;
  long
  identifier(l)
  =
  -
  const int(252645136)
  ;
  unsigned
  int
  *
  identifier(ui_ptr)
  =
  &
  identifier(ui)
  ;
  long
  *
  identifier(l_ptr)
  =
  &
  identifier(l)
  ;
  if
  (
  *
  identifier(ui_ptr)
  ^=
  *
  identifier(l_ptr)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(ui)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(123456)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(l)
  !=
  -
  const int(252645136)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
