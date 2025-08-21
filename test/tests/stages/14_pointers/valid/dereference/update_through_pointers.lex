-- Lexing ... OK
+
+
@@ Tokens @@
List[98]:
  int
  identifier(main)
  (
  void
  )
  {
  unsigned
  int
  identifier(i)
  =
  const unsigned int(2185232384u)
  ;
  signed
  long
  identifier(l)
  =
  const long(144115196665790464l)
  ;
  double
  identifier(d)
  =
  const double(1e50)
  ;
  unsigned
  *
  identifier(i_ptr)
  =
  &
  identifier(i)
  ;
  long
  *
  identifier(l_ptr)
  =
  &
  identifier(l)
  ;
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  *
  identifier(i_ptr)
  =
  const int(10)
  ;
  *
  identifier(l_ptr)
  =
  -
  const int(20)
  ;
  *
  identifier(d_ptr)
  =
  const double(30.1)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(l)
  !=
  -
  const int(20)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(30.1)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
