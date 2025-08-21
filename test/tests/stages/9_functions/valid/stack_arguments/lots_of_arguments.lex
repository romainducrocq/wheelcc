-- Lexing ... OK
+
+
@@ Tokens @@
List[91]:
  int
  identifier(foo)
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
  ,
  int
  identifier(g)
  ,
  int
  identifier(h)
  )
  {
  return
  (
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
  &&
  identifier(g)
  ==
  const int(7)
  &&
  identifier(h)
  ==
  const int(8)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(foo)
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
  const int(6)
  ,
  const int(7)
  ,
  const int(8)
  )
  ;
  }
