-- Lexing ... OK
+
+
@@ Tokens @@
List[98]:
  unsigned
  int
  identifier(double_to_uint)
  (
  double
  identifier(d)
  )
  {
  return
  (
  unsigned
  int
  )
  identifier(d)
  ;
  }
  unsigned
  long
  identifier(double_to_ulong)
  (
  double
  identifier(d)
  )
  {
  return
  (
  unsigned
  long
  )
  identifier(d)
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
  identifier(double_to_uint)
  (
  const double(10.9)
  )
  !=
  const unsigned int(10u)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(double_to_uint)
  (
  const double(2147483750.5)
  )
  !=
  const int(2147483750)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(double_to_ulong)
  (
  const double(34359738368.5)
  )
  !=
  const unsigned long(34359738368ul)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(double_to_ulong)
  (
  const double(3458764513821589504.0)
  )
  !=
  const unsigned long(3458764513821589504ul)
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
