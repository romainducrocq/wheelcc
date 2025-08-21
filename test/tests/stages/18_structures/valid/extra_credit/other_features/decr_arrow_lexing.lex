-- Lexing ... OK
+
+
@@ Tokens @@
List[44]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  =
  {
  const int(0)
  ,
  const int(1)
  ,
  const int(2)
  }
  ;
  int
  *
  identifier(ptr)
  =
  identifier(arr)
  +
  const int(2)
  ;
  if
  (
  identifier(ptr)
  --
  >
  identifier(arr)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
