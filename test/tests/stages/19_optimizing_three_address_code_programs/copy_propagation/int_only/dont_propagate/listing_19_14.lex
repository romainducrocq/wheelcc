-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(indirect_update)
  (
  void
  )
  ;
  int
  identifier(f)
  (
  int
  identifier(new_total)
  )
  {
  static
  int
  identifier(total)
  =
  const int(0)
  ;
  identifier(total)
  =
  identifier(new_total)
  ;
  if
  (
  identifier(total)
  >
  const int(100)
  )
  return
  const int(0)
  ;
  identifier(total)
  =
  const int(10)
  ;
  identifier(indirect_update)
  (
  )
  ;
  return
  identifier(total)
  ;
  }
  int
  identifier(indirect_update)
  (
  void
  )
  {
  identifier(f)
  (
  const int(101)
  )
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
  return
  identifier(f)
  (
  const int(1)
  )
  ;
  }
