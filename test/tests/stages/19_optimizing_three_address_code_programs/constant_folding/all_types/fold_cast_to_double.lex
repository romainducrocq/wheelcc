-- Lexing ... OK
+
+
@@ Tokens @@
List[137]:
  double
  identifier(target_from_int)
  (
  void
  )
  {
  return
  (
  double
  )
  const int(1000)
  ;
  }
  double
  identifier(target_from_uint)
  (
  void
  )
  {
  return
  (
  double
  )
  const unsigned int(4294967290u)
  ;
  }
  double
  identifier(target_from_long)
  (
  void
  )
  {
  return
  (
  double
  )
  const long(4611686018427388416l)
  ;
  }
  double
  identifier(target_from_ulong)
  (
  void
  )
  {
  return
  (
  double
  )
  const unsigned long(9223372036854776833ul)
  ;
  }
  double
  identifier(target_implicit)
  (
  void
  )
  {
  return
  const int(1000)
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
  identifier(target_from_int)
  (
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
  identifier(target_from_uint)
  (
  )
  !=
  const double(4294967290.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_from_long)
  (
  )
  !=
  const double(4611686018427387904.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_from_ulong)
  (
  )
  !=
  const double(9223372036854777856.0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_implicit)
  (
  )
  !=
  const double(1000.0)
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
