-- Lexing ... OK
+
+
@@ Tokens @@
List[482]:
  int
  identifier(target_chars)
  (
  void
  )
  {
  char
  identifier(c)
  =
  const int(100)
  ;
  char
  identifier(c2)
  =
  const int(100)
  ;
  identifier(c)
  +=
  identifier(c2)
  ;
  if
  (
  identifier(c)
  !=
  -
  const int(56)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  char
  identifier(uc)
  =
  const int(200)
  ;
  identifier(c2)
  =
  -
  const int(100)
  ;
  identifier(uc)
  /=
  identifier(c2)
  ;
  if
  (
  identifier(uc)
  !=
  const int(254)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(uc)
  -=
  const double(250.0)
  ;
  if
  (
  identifier(uc)
  !=
  const int(4)
  )
  {
  return
  const int(3)
  ;
  }
  signed
  char
  identifier(sc)
  =
  -
  const int(70)
  ;
  identifier(sc)
  *=
  identifier(c)
  ;
  if
  (
  identifier(sc)
  !=
  const int(80)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  identifier(sc)
  %=
  identifier(c)
  )
  !=
  const int(24)
  )
  {
  return
  const int(5)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_double)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(10.0)
  ;
  identifier(d)
  /=
  const double(4.0)
  ;
  if
  (
  identifier(d)
  !=
  const double(2.5)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(d)
  *=
  const double(10000.0)
  ;
  if
  (
  identifier(d)
  !=
  const double(25000.0)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_double_cast)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(1000.5)
  ;
  identifier(d)
  +=
  const int(1000)
  ;
  if
  (
  identifier(d)
  !=
  const double(2000.5)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551586ul)
  ;
  identifier(ul)
  -=
  const double(1.5E19)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(3446744073709551616ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  const int(10)
  ;
  identifier(i)
  +=
  const double(0.99999)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
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
  int
  identifier(target_uint)
  (
  void
  )
  {
  unsigned
  int
  identifier(x)
  =
  -
  const unsigned int(1u)
  ;
  identifier(x)
  /=
  -
  const long(10l)
  ;
  if
  (
  identifier(x)
  ==
  const unsigned int(3865470567u)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(target_assign_long_to_int)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(20)
  ;
  int
  identifier(b)
  =
  const int(2147483647)
  ;
  int
  identifier(c)
  =
  -
  const int(5000000)
  ;
  identifier(i)
  +=
  const long(2147483648l)
  ;
  if
  (
  identifier(i)
  !=
  const int(2147483628)
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
  const int(2147483647)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(b)
  /=
  -
  const long(34359738367l)
  ;
  if
  (
  identifier(b)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(i)
  !=
  const int(2147483628)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(c)
  !=
  -
  const int(5000000)
  )
  {
  return
  const int(5)
  ;
  }
  identifier(c)
  *=
  const long(10000l)
  ;
  if
  (
  identifier(c)
  !=
  const int(1539607552)
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(target_assign_to_long)
  (
  void
  )
  {
  long
  identifier(l)
  =
  -
  const long(34359738368l)
  ;
  int
  identifier(i)
  =
  -
  const int(10)
  ;
  identifier(l)
  -=
  identifier(i)
  ;
  if
  (
  identifier(l)
  !=
  -
  const long(34359738358l)
  )
  {
  return
  const int(1)
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
  if
  (
  identifier(target_chars)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_double)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_double_cast)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_uint)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_assign_long_to_int)
  (
  )
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(target_assign_to_long)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
