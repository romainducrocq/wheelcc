-- Lexing ... OK
+
+
@@ Tokens @@
List[82]:
  double
  identifier(get_max)
  (
  double
  identifier(a)
  ,
  double
  identifier(b)
  ,
  double
  identifier(c)
  ,
  double
  identifier(d)
  ,
  double
  identifier(e)
  ,
  double
  identifier(f)
  ,
  double
  identifier(g)
  ,
  double
  identifier(h)
  ,
  double
  identifier(i)
  ,
  double
  identifier(j)
  ,
  double
  identifier(k)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(result)
  =
  identifier(get_max)
  (
  const double(100.3)
  ,
  const double(200.1)
  ,
  const double(0.01)
  ,
  const double(1.00004e5)
  ,
  const double(55.555)
  ,
  -
  const double(4.)
  ,
  const double(6543.2)
  ,
  const double(9e9)
  ,
  const double(8e8)
  ,
  const double(7.6)
  ,
  const double(10e3)
  *
  const double(11e5)
  )
  ;
  return
  identifier(result)
  ==
  const double(10e3)
  *
  const double(11e5)
  ;
  }
