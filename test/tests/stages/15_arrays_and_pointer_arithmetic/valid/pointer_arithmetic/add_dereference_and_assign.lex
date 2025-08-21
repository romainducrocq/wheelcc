-- Lexing ... OK
+
+
@@ Tokens @@
List[64]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(2)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  *
  identifier(arr)
  =
  const int(3)
  ;
  *
  (
  identifier(arr)
  +
  const int(1)
  )
  =
  const int(4)
  ;
  if
  (
  identifier(arr)
  [
  const int(0)
  ]
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(arr)
  [
  const int(1)
  ]
  !=
  const int(4)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
