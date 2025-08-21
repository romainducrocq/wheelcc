-- Lexing ... OK
+
+
@@ Tokens @@
List[58]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(tab)
  =
  const char('	')
  ;
  int
  identifier(vertical_tab)
  =
  const char('')
  ;
  int
  identifier(form_feed)
  =
  const char('')
  ;
  if
  (
  identifier(tab)
  !=
  const char('\t')
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(vertical_tab)
  !=
  const char('\v')
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(form_feed)
  !=
  const char('\f')
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
