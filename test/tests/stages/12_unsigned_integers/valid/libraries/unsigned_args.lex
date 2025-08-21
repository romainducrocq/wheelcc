-- Lexing ... OK
+
+
@@ Tokens @@
List[143]:
  int
  identifier(accept_unsigned)
  (
  unsigned
  int
  identifier(a)
  ,
  unsigned
  int
  identifier(b)
  ,
  unsigned
  long
  identifier(c)
  ,
  unsigned
  long
  identifier(d)
  ,
  unsigned
  int
  identifier(e)
  ,
  unsigned
  int
  identifier(f)
  ,
  unsigned
  long
  identifier(g)
  ,
  unsigned
  int
  identifier(h)
  ,
  unsigned
  long
  identifier(i)
  )
  {
  if
  (
  identifier(a)
  !=
  const unsigned int(1u)
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
  const unsigned int(4294967295U)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(c)
  !=
  const unsigned long(18446744073709551615UL)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  const unsigned long(9223372036854775808ul)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(e)
  !=
  const unsigned int(2147483648u)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(f)
  !=
  const unsigned int(0u)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(g)
  !=
  const unsigned int(123456u)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(h)
  !=
  const unsigned int(2147487744u)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(i)
  !=
  const unsigned long(9223372041149743104ul)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
