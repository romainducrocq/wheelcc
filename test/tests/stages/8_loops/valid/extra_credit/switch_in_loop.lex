-- Lexing ... OK
+
+
@@ Tokens @@
List[96]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(acc)
  =
  const int(0)
  ;
  int
  identifier(ctr)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  switch
  (
  identifier(i)
  )
  {
  case
  const int(0)
  :
  identifier(acc)
  =
  const int(2)
  ;
  break
  ;
  case
  const int(1)
  :
  identifier(acc)
  =
  identifier(acc)
  *
  const int(3)
  ;
  break
  ;
  case
  const int(2)
  :
  identifier(acc)
  =
  identifier(acc)
  *
  const int(4)
  ;
  break
  ;
  default
  :
  identifier(acc)
  =
  identifier(acc)
  +
  const int(1)
  ;
  }
  identifier(ctr)
  =
  identifier(ctr)
  +
  const int(1)
  ;
  }
  return
  identifier(ctr)
  ==
  const int(10)
  &&
  identifier(acc)
  ==
  const int(31)
  ;
  }
