-- Lexing ... OK
+
+
@@ Tokens @@
List[228]:
  long
  identifier(a)
  ;
  long
  identifier(b)
  ;
  int
  identifier(addition)
  (
  void
  )
  {
  return
  (
  identifier(a)
  +
  identifier(b)
  ==
  const long(4294967295l)
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
  identifier(a)
  -
  identifier(b)
  ==
  -
  const long(4294967380l)
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
  identifier(a)
  *
  const long(4l)
  ==
  const long(17179869160l)
  )
  ;
  }
  int
  identifier(division)
  (
  void
  )
  {
  identifier(b)
  =
  identifier(a)
  /
  const long(128l)
  ;
  return
  (
  identifier(b)
  ==
  const long(33554431l)
  )
  ;
  }
  int
  identifier(remaind)
  (
  void
  )
  {
  identifier(b)
  =
  -
  identifier(a)
  %
  const long(4294967290l)
  ;
  return
  (
  identifier(b)
  ==
  -
  const long(5l)
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
  identifier(a)
  ==
  -
  const long(9223372036854775807l)
  )
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(a)
  =
  const long(4294967290l)
  ;
  identifier(b)
  =
  const long(5l)
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
  identifier(a)
  =
  -
  const long(4294967290l)
  ;
  identifier(b)
  =
  const long(90l)
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
  identifier(a)
  =
  const long(4294967290l)
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
  identifier(a)
  =
  const long(4294967290l)
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
  identifier(a)
  =
  const long(8589934585l)
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
  const int(5)
  ;
  }
  identifier(a)
  =
  const long(9223372036854775806l)
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
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
