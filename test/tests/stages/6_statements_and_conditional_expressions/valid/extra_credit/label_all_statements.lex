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
  identifier(a)
  =
  const int(1)
  ;
  identifier(label_if)
  :
  if
  (
  identifier(a)
  )
  goto
  identifier(label_expression)
  ;
  else
  goto
  identifier(label_empty)
  ;
  identifier(label_goto)
  :
  goto
  identifier(label_return)
  ;
  if
  (
  const int(0)
  )
  identifier(label_expression)
  :
  identifier(a)
  =
  const int(0)
  ;
  goto
  identifier(label_if)
  ;
  identifier(label_return)
  :
  return
  identifier(a)
  ;
  identifier(label_empty)
  :
  ;
  identifier(a)
  =
  const int(100)
  ;
  goto
  identifier(label_goto)
  ;
  }
