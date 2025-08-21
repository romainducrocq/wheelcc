-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  struct
  identifier(inner)
  {
  int
  identifier(i)
  ;
  }
  ;
  struct
  identifier(outer)
  {
  struct
  identifier(inner)
  identifier(s)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(outer)
  identifier(x)
  =
  {
  {
  const int(1)
  }
  }
  ;
  ++
  identifier(x)
  .
  identifier(s)
  ;
  return
  const int(0)
  ;
  }
