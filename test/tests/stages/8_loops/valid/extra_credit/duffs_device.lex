-- Lexing ... OK
+
+
@@ Tokens @@
List[103]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(count)
  =
  const int(37)
  ;
  int
  identifier(iterations)
  =
  (
  identifier(count)
  +
  const int(4)
  )
  /
  const int(5)
  ;
  switch
  (
  identifier(count)
  %
  const int(5)
  )
  {
  case
  const int(0)
  :
  do
  {
  identifier(count)
  =
  identifier(count)
  -
  const int(1)
  ;
  case
  const int(4)
  :
  identifier(count)
  =
  identifier(count)
  -
  const int(1)
  ;
  case
  const int(3)
  :
  identifier(count)
  =
  identifier(count)
  -
  const int(1)
  ;
  case
  const int(2)
  :
  identifier(count)
  =
  identifier(count)
  -
  const int(1)
  ;
  case
  const int(1)
  :
  identifier(count)
  =
  identifier(count)
  -
  const int(1)
  ;
  }
  while
  (
  (
  identifier(iterations)
  =
  identifier(iterations)
  -
  const int(1)
  )
  >
  const int(0)
  )
  ;
  }
  return
  (
  identifier(count)
  ==
  const int(0)
  &&
  identifier(iterations)
  ==
  const int(0)
  )
  ;
  }
