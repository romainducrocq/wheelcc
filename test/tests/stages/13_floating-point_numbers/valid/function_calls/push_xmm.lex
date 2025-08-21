-- Lexing ... OK
+
+
@@ Tokens @@
List[239]:
  int
  identifier(callee)
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
  {
  if
  (
  identifier(a)
  !=
  const double(0.)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  !=
  const double(1.)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(c)
  !=
  const double(2.)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(3.)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(e)
  !=
  const double(4.)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(f)
  !=
  const double(5.)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(g)
  !=
  const double(6.)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(h)
  !=
  const double(7.)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(i)
  !=
  const double(8.)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(j)
  !=
  const double(9.)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(k)
  !=
  const double(10.)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target)
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
  )
  {
  return
  identifier(callee)
  (
  const double(0.)
  ,
  const double(1.)
  ,
  const double(2.)
  ,
  const double(3.)
  ,
  const double(4.)
  ,
  const double(5.)
  ,
  identifier(e)
  +
  const double(1.)
  ,
  identifier(d)
  +
  const double(3.)
  ,
  identifier(c)
  +
  const double(5.)
  ,
  identifier(b)
  +
  const double(7.)
  ,
  identifier(a)
  +
  const double(9.)
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
  identifier(target)
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
  )
  ;
  }
