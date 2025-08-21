-- Lexing ... OK
+
+
@@ Tokens @@
List[198]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  int
  *
  identifier(y)
  =
  &
  identifier(x)
  ;
  if
  (
  ++
  *
  identifier(y)
  !=
  const int(11)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(11)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  --
  *
  identifier(y)
  !=
  const int(10)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(10)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  *
  identifier(y)
  )
  ++
  !=
  const int(10)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(11)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  (
  *
  identifier(y)
  )
  --
  !=
  const int(11)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(x)
  !=
  const int(10)
  )
  {
  return
  const int(8)
  ;
  }
  unsigned
  long
  identifier(ul)
  =
  const int(0)
  ;
  unsigned
  long
  *
  identifier(ul_ptr)
  =
  &
  identifier(ul)
  ;
  if
  (
  (
  *
  identifier(ul_ptr)
  )
  --
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(ul)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(10)
  ;
  }
  double
  identifier(d)
  =
  const double(0.0)
  ;
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  if
  (
  ++
  (
  *
  identifier(d_ptr)
  )
  !=
  const double(1.0)
  )
  {
  return
  const int(11)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(1.0)
  )
  {
  return
  const int(12)
  ;
  }
  return
  const int(0)
  ;
  }
