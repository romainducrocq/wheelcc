-- Lexing ... OK
+
+
@@ Tokens @@
List[48]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(1)
  ;
  int
  identifier(b)
  =
  const int(2)
  ;
  int
  identifier(c)
  =
  ++
  identifier(a)
  ;
  int
  identifier(d)
  =
  --
  identifier(b)
  ;
  return
  (
  identifier(a)
  ==
  const int(2)
  &&
  identifier(b)
  ==
  const int(1)
  &&
  identifier(c)
  ==
  const int(2)
  &&
  identifier(d)
  ==
  const int(1)
  )
  ;
  }
