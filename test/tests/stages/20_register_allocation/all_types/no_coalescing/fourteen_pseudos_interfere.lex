-- Lexing ... OK
+
+
@@ Tokens @@
List[192]:
  double
  identifier(glob)
  =
  const double(20.0)
  ;
  double
  identifier(glob2)
  =
  const double(30.0)
  ;
  int
  identifier(glob3)
  =
  const double(40.0)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  double
  identifier(a)
  =
  identifier(glob)
  *
  identifier(glob)
  ;
  double
  identifier(b)
  =
  identifier(glob2)
  +
  const double(2.0)
  ;
  double
  identifier(c)
  =
  identifier(a)
  +
  const double(5.0)
  ;
  double
  identifier(d)
  =
  identifier(b)
  -
  identifier(glob3)
  ;
  double
  identifier(e)
  =
  identifier(glob)
  +
  const double(7.0)
  ;
  double
  identifier(f)
  =
  identifier(glob2)
  *
  const double(2.0)
  ;
  double
  identifier(g)
  =
  identifier(c)
  *
  const double(3.0)
  ;
  double
  identifier(h)
  =
  identifier(d)
  *
  const double(112.)
  ;
  double
  identifier(i)
  =
  identifier(e)
  /
  const double(3.0)
  ;
  double
  identifier(j)
  =
  identifier(g)
  +
  identifier(f)
  ;
  double
  identifier(k)
  =
  identifier(h)
  -
  identifier(j)
  ;
  double
  identifier(l)
  =
  identifier(i)
  +
  const double(1000.)
  ;
  double
  identifier(m)
  =
  identifier(j)
  -
  identifier(d)
  ;
  double
  identifier(n)
  =
  identifier(m)
  *
  identifier(l)
  ;
  if
  (
  identifier(a)
  ==
  const double(400.0)
  &&
  identifier(b)
  ==
  const double(32.0)
  &&
  identifier(c)
  ==
  const double(405.0)
  &&
  identifier(d)
  ==
  -
  const double(8.0)
  &&
  identifier(e)
  ==
  const double(27.0)
  &&
  identifier(f)
  ==
  const double(60.0)
  &&
  identifier(g)
  ==
  const double(1215.0)
  &&
  identifier(h)
  ==
  -
  const double(896.)
  &&
  identifier(i)
  ==
  const double(9.0)
  &&
  identifier(j)
  ==
  const double(1275.)
  &&
  identifier(k)
  ==
  -
  const double(2171.)
  &&
  identifier(l)
  ==
  const double(1009.)
  &&
  identifier(m)
  ==
  const double(1283.)
  &&
  identifier(n)
  ==
  const double(1294547.)
  )
  {
  return
  const int(0)
  ;
  }
  else
  {
  return
  const int(1)
  ;
  }
  }
