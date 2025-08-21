-- Lexing ... OK
+
+
@@ Tokens @@
List[62]:
  int
  identifier(multi_path)
  (
  int
  identifier(flag)
  )
  {
  int
  identifier(x)
  =
  const int(3)
  ;
  if
  (
  identifier(flag)
  )
  identifier(x)
  =
  const int(4)
  ;
  return
  identifier(x)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(multi_path)
  (
  const int(1)
  )
  !=
  const int(4)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(multi_path)
  (
  const int(0)
  )
  !=
  const int(3)
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
