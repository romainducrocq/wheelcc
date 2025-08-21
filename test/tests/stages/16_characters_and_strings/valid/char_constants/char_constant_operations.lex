-- Lexing ... OK
+
+
@@ Tokens @@
List[199]:
  double
  identifier(d)
  =
  const char('\\')
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(d)
  !=
  const double(92.0)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  long
  identifier(array)
  [
  const char('\n')
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const char('a')
  ,
  const char('\b')
  ,
  const int(3)
  ,
  const int(4)
  ,
  const int(5)
  ,
  const char('!')
  ,
  const char('%')
  ,
  const char('~')
  }
  ;
  if
  (
  identifier(array)
  [
  const int(2)
  ]
  !=
  const int(97)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(array)
  [
  const int(3)
  ]
  !=
  const int(8)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(array)
  [
  const int(7)
  ]
  !=
  const int(33)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(array)
  [
  const int(8)
  ]
  !=
  const int(37)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(array)
  [
  const int(9)
  ]
  !=
  const int(126)
  )
  {
  return
  const int(6)
  ;
  }
  unsigned
  long
  (
  *
  identifier(array_ptr)
  )
  [
  const int(10)
  ]
  =
  &
  identifier(array)
  ;
  if
  (
  identifier(array_ptr)
  [
  const int(0)
  ]
  [
  const int(9)
  ]
  !=
  const char('~')
  )
  {
  return
  const int(7)
  ;
  }
  int
  identifier(i)
  =
  identifier(array)
  [
  const char('\a')
  ]
  ;
  if
  (
  identifier(i)
  !=
  const int(33)
  )
  {
  return
  const int(8)
  ;
  }
  double
  identifier(d)
  =
  const int(10)
  %
  const char('\a')
  +
  const double(4.0)
  *
  const char('_')
  -
  ~
  const char('@')
  ;
  if
  (
  identifier(d)
  !=
  const double(448.0)
  )
  {
  return
  const int(9)
  ;
  }
  return
  const int(0)
  ;
  }
