-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const int(0)
  ;
  while
  (
  const int(1)
  )
  {
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  >
  const int(10)
  )
  break
  ;
  }
  int
  identifier(j)
  =
  const int(10)
  ;
  while
  (
  const int(1)
  )
  {
  identifier(j)
  =
  identifier(j)
  -
  const int(1)
  ;
  if
  (
  identifier(j)
  <
  const int(0)
  )
  break
  ;
  }
  int
  identifier(result)
  =
  identifier(j)
  ==
  -
  const int(1)
  &&
  identifier(i)
  ==
  const int(11)
  ;
  return
  identifier(result)
  ;
  }
