-- Lexing ... OK
+
+
@@ Tokens @@
List[62]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(100)
  ;
  int
  identifier(count)
  =
  const int(0)
  ;
  while
  (
  identifier(i)
  --
  )
  identifier(count)
  ++
  ;
  if
  (
  identifier(count)
  !=
  const int(100)
  )
  return
  const int(0)
  ;
  identifier(i)
  =
  const int(100)
  ;
  identifier(count)
  =
  const int(0)
  ;
  while
  (
  --
  identifier(i)
  )
  identifier(count)
  ++
  ;
  if
  (
  identifier(count)
  !=
  const int(99)
  )
  return
  const int(0)
  ;
  return
  const int(1)
  ;
  }
