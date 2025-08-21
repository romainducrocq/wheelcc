-- Lexing ... OK
+
+
@@ Tokens @@
List[125]:
  unsigned
  int
  identifier(ui_a)
  ;
  unsigned
  int
  identifier(ui_b)
  ;
  unsigned
  long
  identifier(ul_a)
  ;
  unsigned
  long
  identifier(ul_b)
  ;
  int
  identifier(addition)
  (
  void
  )
  {
  return
  identifier(ui_a)
  +
  identifier(ui_b)
  ==
  const unsigned int(0u)
  ;
  }
  int
  identifier(subtraction)
  (
  void
  )
  {
  return
  (
  identifier(ul_a)
  -
  identifier(ul_b)
  ==
  const unsigned long(18446744073709551606ul)
  )
  ;
  }
  int
  identifier(neg)
  (
  void
  )
  {
  return
  -
  identifier(ul_a)
  ==
  const unsigned long(18446744073709551615UL)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(ui_a)
  =
  const unsigned int(4294967293u)
  ;
  identifier(ui_b)
  =
  const unsigned int(3u)
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
  identifier(ul_a)
  =
  const unsigned long(10ul)
  ;
  identifier(ul_b)
  =
  const unsigned long(20ul)
  ;
  if
  (
  !
  identifier(subtraction)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  identifier(ul_a)
  =
  const unsigned long(1ul)
  ;
  if
  (
  !
  identifier(neg)
  (
  )
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
