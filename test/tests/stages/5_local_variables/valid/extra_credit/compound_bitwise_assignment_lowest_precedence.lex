-- Lexing ... OK
+
+
@@ Tokens @@
List[85]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(11)
  ;
  int
  identifier(b)
  =
  const int(12)
  ;
  identifier(a)
  &=
  const int(0)
  ||
  identifier(b)
  ;
  identifier(b)
  ^=
  identifier(a)
  ||
  const int(1)
  ;
  int
  identifier(c)
  =
  const int(14)
  ;
  identifier(c)
  |=
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
  >>=
  identifier(c)
  ||
  identifier(d)
  ;
  int
  identifier(e)
  =
  const int(18)
  ;
  identifier(e)
  <<=
  identifier(c)
  ||
  identifier(d)
  ;
  return
  (
  identifier(a)
  ==
  const int(1)
  &&
  identifier(b)
  ==
  const int(13)
  &&
  identifier(c)
  ==
  const int(15)
  &&
  identifier(d)
  ==
  const int(8)
  &&
  identifier(e)
  ==
  const int(36)
  )
  ;
  }
