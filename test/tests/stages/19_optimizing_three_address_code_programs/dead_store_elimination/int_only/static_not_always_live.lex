-- Lexing ... OK
+
+
@@ Tokens @@
List[90]:
  int
  identifier(exit_wrapper)
  (
  int
  identifier(status)
  )
  ;
  int
  identifier(i)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  identifier(i)
  =
  const int(30)
  ;
  int
  identifier(counter)
  =
  const int(0)
  ;
  do
  {
  if
  (
  identifier(counter)
  <
  const int(10)
  )
  {
  identifier(i)
  =
  identifier(counter)
  +
  const int(1)
  ;
  }
  else
  {
  identifier(i)
  =
  identifier(counter)
  +
  const int(2)
  ;
  }
  if
  (
  identifier(counter)
  >
  const int(20)
  )
  {
  identifier(exit_wrapper)
  (
  identifier(i)
  )
  ;
  }
  identifier(counter)
  =
  identifier(counter)
  +
  const int(1)
  ;
  }
  while
  (
  const int(1)
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
  identifier(target)
  (
  )
  ;
  }
