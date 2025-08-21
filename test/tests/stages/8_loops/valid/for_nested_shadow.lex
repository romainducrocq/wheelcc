-- Lexing ... OK
+
+
@@ Tokens @@
List[70]:
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
  int
  identifier(j)
  =
  const int(0)
  ;
  int
  identifier(k)
  =
  const int(1)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(100)
  ;
  identifier(i)
  >
  const int(0)
  ;
  identifier(i)
  =
  identifier(i)
  -
  const int(1)
  )
  {
  int
  identifier(i)
  =
  const int(1)
  ;
  int
  identifier(j)
  =
  identifier(i)
  +
  identifier(k)
  ;
  identifier(k)
  =
  identifier(j)
  ;
  }
  return
  identifier(k)
  ==
  const int(101)
  &&
  identifier(i)
  ==
  const int(0)
  &&
  identifier(j)
  ==
  const int(0)
  ;
  }
