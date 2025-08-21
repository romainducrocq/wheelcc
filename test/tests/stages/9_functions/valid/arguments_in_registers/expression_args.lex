-- Lexing ... OK
+
+
@@ Tokens @@
List[38]:
  int
  identifier(sub)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  )
  {
  return
  identifier(a)
  -
  identifier(b)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  identifier(sub)
  (
  const int(1)
  +
  const int(2)
  ,
  const int(1)
  )
  ;
  return
  identifier(sum)
  ;
  }
