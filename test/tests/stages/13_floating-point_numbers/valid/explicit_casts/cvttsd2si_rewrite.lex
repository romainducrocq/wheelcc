-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
  double
  identifier(glob)
  =
  const double(3.0)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  long
  identifier(l)
  =
  -
  const long(1l)
  ;
  int
  identifier(i)
  =
  -
  const int(1)
  ;
  int
  identifier(j)
  =
  (
  int
  )
  identifier(glob)
  ;
  int
  identifier(k)
  =
  const int(20)
  ;
  if
  (
  identifier(l)
  !=
  -
  const long(1l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(i)
  !=
  -
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(j)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(k)
  !=
  const int(20)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
