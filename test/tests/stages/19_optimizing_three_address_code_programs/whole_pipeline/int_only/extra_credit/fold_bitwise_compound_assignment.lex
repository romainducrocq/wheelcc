-- Lexing ... OK
+
+
@@ Tokens @@
List[101]:
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(v)
  =
  -
  const int(100)
  ;
  int
  identifier(w)
  =
  const int(100)
  ;
  int
  identifier(x)
  =
  const int(200)
  ;
  int
  identifier(y)
  =
  const int(300)
  ;
  int
  identifier(z)
  =
  const int(40000)
  ;
  identifier(v)
  ^=
  const int(10)
  ;
  identifier(w)
  |=
  identifier(v)
  ;
  identifier(x)
  &=
  const int(30)
  ;
  identifier(y)
  <<=
  identifier(x)
  ;
  identifier(z)
  >>=
  (
  identifier(x)
  |=
  const int(2)
  )
  ;
  if
  (
  identifier(v)
  ==
  -
  const int(106)
  &&
  identifier(w)
  ==
  -
  const int(10)
  &&
  identifier(x)
  ==
  const int(10)
  &&
  identifier(y)
  ==
  const int(76800)
  &&
  identifier(z)
  ==
  const int(39)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
