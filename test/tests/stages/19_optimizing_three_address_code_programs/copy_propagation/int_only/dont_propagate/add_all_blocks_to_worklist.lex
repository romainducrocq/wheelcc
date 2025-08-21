-- Lexing ... OK
+
+
@@ Tokens @@
List[46]:
  int
  identifier(global)
  ;
  int
  identifier(flag)
  =
  const int(1)
  ;
  int
  identifier(f)
  (
  void
  )
  {
  identifier(global)
  =
  const int(100)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(global)
  =
  const int(0)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(f)
  (
  )
  ;
  }
  return
  identifier(global)
  ;
  }
