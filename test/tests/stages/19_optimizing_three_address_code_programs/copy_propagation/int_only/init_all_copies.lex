-- Lexing ... OK
+
+
@@ Tokens @@
List[89]:
  int
  identifier(counter)
  =
  const int(0)
  ;
  int
  identifier(increment_counter)
  (
  void
  )
  {
  identifier(counter)
  =
  identifier(counter)
  +
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(y)
  =
  const int(3)
  ;
  do
  {
  identifier(increment_counter)
  (
  )
  ;
  }
  while
  (
  identifier(counter)
  <
  const int(5)
  )
  ;
  return
  identifier(y)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(result)
  =
  identifier(target)
  (
  )
  ;
  if
  (
  identifier(result)
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
  identifier(counter)
  !=
  const int(5)
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
