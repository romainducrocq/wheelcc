-- Lexing ... OK
+
+
@@ Tokens @@
List[109]:
  unsigned
  int
  identifier(ui)
  =
  const int(4294967295)
  ;
  unsigned
  int
  *
  identifier(get_ui_ptr)
  (
  void
  )
  {
  return
  &
  identifier(ui)
  ;
  }
  int
  identifier(shiftcount)
  =
  const int(5)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  (
  *
  identifier(get_ui_ptr)
  (
  )
  <<
  const long(2l)
  )
  !=
  const int(4294967292)
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
  identifier(get_ui_ptr)
  (
  )
  >>
  const int(2)
  )
  !=
  const int(1073741823)
  )
  {
  return
  const int(2)
  ;
  }
  int
  *
  identifier(shiftcount_ptr)
  =
  &
  identifier(shiftcount)
  ;
  if
  (
  (
  const unsigned int(1000000u)
  >>
  *
  identifier(shiftcount_ptr)
  )
  !=
  const int(31250)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  const unsigned int(1000000u)
  <<
  *
  identifier(shiftcount_ptr)
  )
  !=
  const int(32000000)
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
