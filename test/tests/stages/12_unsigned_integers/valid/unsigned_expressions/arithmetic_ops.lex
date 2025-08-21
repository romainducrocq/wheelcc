-- Lexing ... OK
+
+
@@ Tokens @@
List[301]:
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
  (
  identifier(ui_a)
  +
  const unsigned int(2147483653u)
  ==
  const unsigned int(2147483663u)
  )
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
  const unsigned long(18446744072635808792ul)
  )
  ;
  }
  int
  identifier(multiplication)
  (
  void
  )
  {
  return
  (
  identifier(ui_a)
  *
  identifier(ui_b)
  ==
  const unsigned int(3221225472u)
  )
  ;
  }
  int
  identifier(division)
  (
  void
  )
  {
  return
  (
  identifier(ui_a)
  /
  identifier(ui_b)
  ==
  const int(0)
  )
  ;
  }
  int
  identifier(division_large_dividend)
  (
  void
  )
  {
  return
  (
  identifier(ui_a)
  /
  identifier(ui_b)
  ==
  const int(2)
  )
  ;
  }
  int
  identifier(division_by_literal)
  (
  void
  )
  {
  return
  (
  identifier(ul_a)
  /
  const unsigned long(5ul)
  ==
  const unsigned long(219902325555ul)
  )
  ;
  }
  int
  identifier(remaind)
  (
  void
  )
  {
  return
  (
  identifier(ul_b)
  %
  identifier(ul_a)
  ==
  const unsigned long(5ul)
  )
  ;
  }
  int
  identifier(complement)
  (
  void
  )
  {
  return
  (
  ~
  identifier(ui_a)
  ==
  const int(0)
  )
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
  const unsigned int(10u)
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
  const unsigned long(18446744072635809792ul)
  ;
  identifier(ul_b)
  =
  const unsigned long(1000ul)
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
  identifier(ui_a)
  =
  const unsigned int(1073741824u)
  ;
  identifier(ui_b)
  =
  const unsigned int(3u)
  ;
  if
  (
  !
  identifier(multiplication)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  identifier(ui_a)
  =
  const unsigned int(100u)
  ;
  identifier(ui_b)
  =
  const unsigned int(4294967294u)
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
  const int(4)
  ;
  }
  identifier(ui_a)
  =
  const unsigned int(4294967294u)
  ;
  identifier(ui_b)
  =
  const unsigned int(2147483647u)
  ;
  if
  (
  !
  identifier(division_large_dividend)
  (
  )
  )
  {
  return
  const int(5)
  ;
  }
  identifier(ul_a)
  =
  const unsigned long(1099511627775ul)
  ;
  if
  (
  !
  identifier(division_by_literal)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  identifier(ul_a)
  =
  const unsigned long(100ul)
  ;
  identifier(ul_b)
  =
  const unsigned long(18446744073709551605ul)
  ;
  if
  (
  !
  identifier(remaind)
  (
  )
  )
  {
  return
  const int(7)
  ;
  }
  identifier(ui_a)
  =
  const unsigned int(4294967295U)
  ;
  if
  (
  !
  identifier(complement)
  (
  )
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
