-- Lexing ... OK
+
+
@@ Tokens @@
List[39]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  }
  ;
  struct
  identifier(s)
  identifier(x)
  =
  {
  const int(1)
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(s)
  *
  identifier(ptr)
  =
  &
  identifier(x)
  ;
  *
  identifier(ptr)
  =
  const int(2)
  ;
  return
  const int(0)
  ;
  }
