-- Lexing ... OK
+
+
@@ Tokens @@
List[82]:
  int
  identifier(x)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  int
  identifier(f)
  )
  {
  return
  identifier(a)
  ==
  const int(1)
  &&
  identifier(b)
  ==
  const int(2)
  &&
  identifier(c)
  ==
  const int(3)
  &&
  identifier(d)
  ==
  const int(4)
  &&
  identifier(e)
  ==
  const int(5)
  &&
  identifier(f)
  ==
  const int(6)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(a)
  =
  const int(4)
  ;
  return
  identifier(x)
  (
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  ,
  const int(24)
  >>
  (
  identifier(a)
  /
  const int(2)
  )
  )
  ;
  }
