-- Lexing ... OK
+
+
@@ Tokens @@
List[61]:
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(nested)
  [
  const int(3)
  ]
  [
  const int(23)
  ]
  =
  {
  {
  const int(0)
  ,
  const int(1)
  }
  ,
  {
  const int(2)
  }
  }
  ;
  int
  (
  *
  identifier(ptr)
  )
  [
  const int(23)
  ]
  =
  identifier(nested)
  ;
  identifier(ptr)
  ++
  ;
  return
  *
  identifier(ptr)
  [
  const int(0)
  ]
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
