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
  identifier(a)
  =
  const int(10)
  ;
  int
  identifier(b)
  =
  const int(12)
  ;
  identifier(a)
  +=
  const int(0)
  ||
  identifier(b)
  ;
  identifier(b)
  *=
  identifier(a)
  &&
  const int(0)
  ;
  int
  identifier(c)
  =
  const int(14)
  ;
  identifier(c)
  -=
  identifier(a)
  ||
  identifier(b)
  ;
  int
  identifier(d)
  =
  const int(16)
  ;
  identifier(d)
  /=
  identifier(c)
  ||
  identifier(d)
  ;
  return
  (
  identifier(a)
  ==
  const int(11)
  &&
  identifier(b)
  ==
  const int(0)
  &&
  identifier(c)
  ==
  const int(13)
  &&
  identifier(d)
  ==
  const int(16)
  )
  ;
  }
