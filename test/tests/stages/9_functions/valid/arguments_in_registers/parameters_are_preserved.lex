-- Lexing ... OK
+
+
@@ Tokens @@
List[122]:
  int
  identifier(g)
  (
  int
  identifier(w)
  ,
  int
  identifier(x)
  ,
  int
  identifier(y)
  ,
  int
  identifier(z)
  )
  {
  if
  (
  identifier(w)
  ==
  const int(2)
  &&
  identifier(x)
  ==
  const int(4)
  &&
  identifier(y)
  ==
  const int(6)
  &&
  identifier(z)
  ==
  const int(8)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(f)
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
  )
  {
  int
  identifier(result)
  =
  identifier(g)
  (
  identifier(a)
  *
  const int(2)
  ,
  identifier(b)
  *
  const int(2)
  ,
  identifier(c)
  *
  const int(2)
  ,
  identifier(d)
  *
  const int(2)
  )
  ;
  return
  (
  identifier(result)
  ==
  const int(1)
  &&
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
  identifier(f)
  (
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  ,
  const int(4)
  )
  ;
  }
