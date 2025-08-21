-- Lexing ... OK
+
+
@@ Tokens @@
List[59]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(counter)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  identifier(counter)
  <
  const int(3)
  ;
  identifier(counter)
  =
  identifier(counter)
  +
  const int(1)
  )
  {
  if
  (
  identifier(i)
  [
  identifier(counter)
  ]
  !=
  identifier(counter)
  +
  const int(1)
  )
  {
  return
  const int(1)
  ;
  }
  }
  return
  const int(0)
  ;
  }
