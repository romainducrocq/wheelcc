-- Lexing ... OK
+
+
@@ Tokens @@
List[61]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  void
  identifier(free)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  *
  identifier(array)
  =
  identifier(malloc)
  (
  const int(10)
  *
  sizeof
  (
  int
  )
  )
  ;
  identifier(array)
  [
  const int(2)
  ]
  =
  const int(100)
  ;
  int
  identifier(result)
  =
  identifier(array)
  [
  const int(2)
  ]
  ;
  identifier(free)
  (
  identifier(array)
  )
  ;
  return
  identifier(result)
  ;
  }
