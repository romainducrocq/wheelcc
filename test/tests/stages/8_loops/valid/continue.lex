-- Lexing ... OK
+
+
@@ Tokens @@
List[63]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  const int(0)
  ;
  int
  identifier(counter)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <=
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(counter)
  =
  identifier(i)
  ;
  if
  (
  identifier(i)
  %
  const int(2)
  ==
  const int(0)
  )
  continue
  ;
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  }
  return
  identifier(sum)
  ==
  const int(5)
  &&
  identifier(counter)
  ==
  const int(10)
  ;
  }
