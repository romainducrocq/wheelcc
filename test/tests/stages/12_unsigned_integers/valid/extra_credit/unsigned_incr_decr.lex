-- Lexing ... OK
+
+
@@ Tokens @@
List[114]:
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
  const int(0)
  ;
  if
  (
  identifier(i)
  --
  !=
  const int(0)
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
  const unsigned int(4294967295U)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  --
  identifier(i)
  !=
  const unsigned int(4294967294U)
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
  const unsigned int(4294967294U)
  )
  {
  return
  const int(4)
  ;
  }
  unsigned
  long
  identifier(l)
  =
  const unsigned long(18446744073709551614UL)
  ;
  if
  (
  identifier(l)
  ++
  !=
  const unsigned long(18446744073709551614UL)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(l)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  ++
  identifier(l)
  !=
  const int(0)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(l)
  !=
  const int(0)
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
