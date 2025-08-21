-- Lexing ... OK
+
+
@@ Tokens @@
List[98]:
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
  const int(400)
  ;
  identifier(v)
  +=
  const int(10)
  ;
  identifier(w)
  -=
  const int(20)
  ;
  identifier(x)
  *=
  const int(30)
  ;
  identifier(y)
  /=
  const int(100)
  ;
  identifier(z)
  %=
  identifier(y)
  +=
  const int(6)
  ;
  if
  (
  identifier(v)
  ==
  -
  const int(90)
  &&
  identifier(w)
  ==
  const int(80)
  &&
  identifier(x)
  ==
  const int(6000)
  &&
  identifier(y)
  ==
  const int(9)
  &&
  identifier(z)
  ==
  const int(4)
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
