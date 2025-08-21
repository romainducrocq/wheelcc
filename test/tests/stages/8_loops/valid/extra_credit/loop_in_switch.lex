-- Lexing ... OK
+
+
@@ Tokens @@
List[71]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(cond)
  =
  const int(10)
  ;
  switch
  (
  identifier(cond)
  )
  {
  case
  const int(1)
  :
  return
  const int(0)
  ;
  case
  const int(10)
  :
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(5)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(cond)
  =
  identifier(cond)
  -
  const int(1)
  ;
  if
  (
  identifier(cond)
  ==
  const int(8)
  )
  break
  ;
  }
  return
  const int(123)
  ;
  default
  :
  return
  const int(2)
  ;
  }
  return
  const int(3)
  ;
  }
