-- Lexing ... OK
+
+
@@ Tokens @@
List[227]:
  int
  identifier(check_arguments)
  (
  double
  identifier(d1)
  ,
  double
  identifier(d2)
  ,
  int
  identifier(i1)
  ,
  double
  identifier(d3)
  ,
  double
  identifier(d4)
  ,
  int
  identifier(i2)
  ,
  int
  identifier(i3)
  ,
  int
  identifier(i4)
  ,
  double
  identifier(d5)
  ,
  double
  identifier(d6)
  ,
  double
  identifier(d7)
  ,
  int
  identifier(i5)
  ,
  double
  identifier(d8)
  )
  {
  if
  (
  identifier(d1)
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
  identifier(d2)
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
  identifier(d3)
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
  identifier(d4)
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
  identifier(d5)
  !=
  const double(5.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(d6)
  !=
  const double(6.0)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(d7)
  !=
  const double(7.0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(d8)
  !=
  const double(8.0)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(i1)
  !=
  const int(101)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(i2)
  !=
  const int(102)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(i3)
  !=
  const int(103)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(i4)
  !=
  const int(104)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  identifier(i5)
  !=
  const int(105)
  )
  {
  return
  const int(13)
  ;
  }
  return
  const int(0)
  ;
  }
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
  const int(101)
  ,
  const double(3.0)
  ,
  const double(4.0)
  ,
  const int(102)
  ,
  const int(103)
  ,
  const int(104)
  ,
  const double(5.0)
  ,
  const double(6.0)
  ,
  const double(7.0)
  ,
  const int(105)
  ,
  const double(8.0)
  )
  ;
  }
