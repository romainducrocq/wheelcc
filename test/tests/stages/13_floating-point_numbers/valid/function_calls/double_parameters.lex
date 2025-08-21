-- Lexing ... OK
+
+
@@ Tokens @@
List[183]:
  int
  identifier(check_arguments)
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
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(check_arguments)
  (
  const double(1.0)
  ,
  const double(2.0)
  ,
  const double(3.0)
  ,
  const double(4.0)
  ,
  -
  const double(1.0)
  ,
  -
  const double(2.0)
  ,
  -
  const double(3.0)
  ,
  -
  const double(4.0)
  )
  ;
  }
  int
  identifier(check_arguments)
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
  )
  {
  if
  (
  identifier(a)
  !=
  const double(1.0)
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
  const double(2.0)
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
  const double(3.0)
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
  const double(4.0)
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
  -
  const double(1.0)
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
  -
  const double(2.0)
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
  -
  const double(3.0)
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
  -
  const double(4.0)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
