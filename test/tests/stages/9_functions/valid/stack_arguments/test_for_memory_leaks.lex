-- Lexing ... OK
+
+
@@ Tokens @@
List[126]:
  int
  identifier(lots_of_args)
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
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(k)
  ,
  int
  identifier(l)
  ,
  int
  identifier(m)
  ,
  int
  identifier(n)
  ,
  int
  identifier(o)
  )
  {
  return
  identifier(l)
  +
  identifier(o)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(ret)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(10000000)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(ret)
  =
  identifier(lots_of_args)
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
  ,
  const int(9)
  ,
  const int(10)
  ,
  const int(11)
  ,
  identifier(ret)
  ,
  const int(13)
  ,
  const int(14)
  ,
  const int(15)
  )
  ;
  }
  return
  identifier(ret)
  ==
  const int(150000000)
  ;
  }
