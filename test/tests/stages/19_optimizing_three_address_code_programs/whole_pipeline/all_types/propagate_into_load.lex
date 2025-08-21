-- Lexing ... OK
+
+
@@ Tokens @@
List[66]:
  int
  *
  identifier(glob)
  ;
  int
  identifier(i)
  =
  const int(10)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  *
  identifier(loc)
  =
  &
  identifier(i)
  ;
  identifier(glob)
  =
  identifier(loc)
  ;
  return
  *
  identifier(glob)
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
  identifier(target)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  identifier(glob)
  !=
  const int(10)
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
