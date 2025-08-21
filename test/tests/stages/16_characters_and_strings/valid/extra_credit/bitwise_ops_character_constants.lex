-- Lexing ... OK
+
+
@@ Tokens @@
List[104]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  if
  (
  (
  identifier(x)
  ^
  const char('A')
  )
  !=
  const int(75)
  )
  {
  return
  const int(1)
  ;
  }
  static
  char
  identifier(c)
  =
  const int(132)
  ;
  if
  (
  (
  const char('!')
  |
  identifier(c)
  )
  !=
  -
  const int(91)
  )
  {
  return
  const int(2)
  ;
  }
  static
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(9259400834947493926ul)
  ;
  if
  (
  (
  identifier(ul)
  &
  const char('~')
  )
  !=
  const int(38)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  (
  identifier(ul)
  <<
  const char(' ')
  )
  !=
  const unsigned long(4611738958194278400ul)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  (
  const char('{')
  >>
  const int(3)
  )
  !=
  const int(15)
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
