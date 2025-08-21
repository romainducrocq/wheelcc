-- Lexing ... OK
+
+
@@ Tokens @@
List[41]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(1)
  ;
  goto
  identifier(post_declaration)
  ;
  int
  identifier(i)
  =
  (
  identifier(x)
  =
  const int(0)
  )
  ;
  identifier(post_declaration)
  :
  identifier(i)
  =
  const int(5)
  ;
  return
  (
  identifier(x)
  ==
  const int(1)
  &&
  identifier(i)
  ==
  const int(5)
  )
  ;
  }
