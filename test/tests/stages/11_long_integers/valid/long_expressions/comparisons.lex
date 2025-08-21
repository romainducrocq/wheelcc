-- Lexing ... OK
+
+
@@ Tokens @@
List[177]:
  long
  identifier(l)
  ;
  long
  identifier(l2)
  ;
  int
  identifier(compare_constants)
  (
  void
  )
  {
  return
  const long(8589934593l)
  >
  const long(255l)
  ;
  }
  int
  identifier(compare_constants_2)
  (
  void
  )
  {
  return
  const long(255l)
  <
  const long(8589934593l)
  ;
  }
  int
  identifier(l_geq_2_60)
  (
  void
  )
  {
  return
  (
  identifier(l)
  >=
  const long(1152921504606846976l)
  )
  ;
  }
  int
  identifier(uint_max_leq_l)
  (
  void
  )
  {
  return
  (
  const long(4294967295l)
  <=
  identifier(l)
  )
  ;
  }
  int
  identifier(l_eq_l2)
  (
  void
  )
  {
  return
  (
  identifier(l)
  ==
  identifier(l2)
  )
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
  !
  identifier(compare_constants)
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
  !
  identifier(compare_constants_2)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  identifier(l)
  =
  -
  const long(9223372036854775807l)
  ;
  if
  (
  identifier(l_geq_2_60)
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
  identifier(uint_max_leq_l)
  (
  )
  )
  {
  return
  const int(4)
  ;
  }
  identifier(l)
  =
  const long(1152921504606846976l)
  ;
  if
  (
  !
  identifier(l_geq_2_60)
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
  !
  identifier(uint_max_leq_l)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  identifier(l2)
  =
  identifier(l)
  ;
  if
  (
  !
  identifier(l_eq_l2)
  (
  )
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
