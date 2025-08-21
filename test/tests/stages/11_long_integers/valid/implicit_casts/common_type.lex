-- Lexing ... OK
+
+
@@ Tokens @@
List[172]:
  long
  identifier(l)
  ;
  int
  identifier(i)
  ;
  int
  identifier(addition)
  (
  void
  )
  {
  long
  identifier(result)
  =
  identifier(i)
  +
  identifier(l)
  ;
  return
  (
  identifier(result)
  ==
  const long(2147483663l)
  )
  ;
  }
  int
  identifier(division)
  (
  void
  )
  {
  int
  identifier(int_result)
  =
  identifier(l)
  /
  identifier(i)
  ;
  return
  (
  identifier(int_result)
  ==
  const int(214748364)
  )
  ;
  }
  int
  identifier(comparison)
  (
  void
  )
  {
  return
  (
  identifier(i)
  <=
  identifier(l)
  )
  ;
  }
  int
  identifier(conditional)
  (
  void
  )
  {
  long
  identifier(result)
  =
  const int(1)
  ?
  identifier(l)
  :
  identifier(i)
  ;
  return
  (
  identifier(result)
  ==
  const long(8589934592l)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(l)
  =
  const int(2147483653)
  ;
  identifier(i)
  =
  const int(10)
  ;
  if
  (
  !
  identifier(addition)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  identifier(l)
  =
  const long(2147483649l)
  ;
  if
  (
  !
  identifier(division)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  identifier(i)
  =
  -
  const int(100)
  ;
  identifier(l)
  =
  const int(2147483648)
  ;
  if
  (
  !
  identifier(comparison)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  identifier(l)
  =
  const long(8589934592l)
  ;
  identifier(i)
  =
  const int(10)
  ;
  if
  (
  !
  identifier(conditional)
  (
  )
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
