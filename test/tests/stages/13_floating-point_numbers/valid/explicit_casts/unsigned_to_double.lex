-- Lexing ... OK
+
+
@@ Tokens @@
List[152]:
  double
  identifier(uint_to_double)
  (
  unsigned
  int
  identifier(ui)
  )
  {
  return
  (
  double
  )
  identifier(ui)
  ;
  }
  double
  identifier(ulong_to_double)
  (
  unsigned
  long
  identifier(ul)
  )
  {
  return
  (
  double
  )
  identifier(ul)
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
  identifier(uint_to_double)
  (
  const unsigned int(1000u)
  )
  !=
  const double(1000.0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uint_to_double)
  (
  const unsigned int(4294967200u)
  )
  !=
  const double(4294967200.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(138512825844ul)
  )
  !=
  const double(138512825844.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(10223372036854775816ul)
  )
  !=
  const double(10223372036854775808.0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(9223372036854776832ul)
  )
  !=
  const double(9223372036854775808.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(9223372036854776833ul)
  )
  !=
  const double(9223372036854777856.0)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(9223372036854776831ul)
  )
  !=
  const double(9223372036854775808.0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(ulong_to_double)
  (
  const unsigned long(9223372036854776830ul)
  )
  !=
  const double(9223372036854775808.0)
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
