-- Lexing ... OK
+
+
@@ Tokens @@
List[138]:
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(5.0)
  ;
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  *
  identifier(d_ptr)
  *=
  const unsigned int(1000u)
  ;
  if
  (
  identifier(d)
  !=
  const double(5000.0)
  )
  {
  return
  const int(1)
  ;
  }
  int
  identifier(i)
  =
  -
  const int(50)
  ;
  int
  *
  identifier(i_ptr)
  =
  &
  identifier(i)
  ;
  *
  identifier(i_ptr)
  %=
  const unsigned int(4294967200U)
  ;
  if
  (
  *
  identifier(i_ptr)
  !=
  const int(46)
  )
  {
  return
  const int(2)
  ;
  }
  unsigned
  int
  identifier(ui)
  =
  const unsigned int(4294967295U)
  ;
  identifier(ui)
  /=
  *
  identifier(d_ptr)
  ;
  if
  (
  identifier(ui)
  !=
  const unsigned int(858993u)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(i)
  =
  -
  const int(10)
  ;
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(9223372036854775807ul)
  ;
  unsigned
  long
  *
  identifier(ul_ptr)
  =
  &
  identifier(ul)
  ;
  *
  identifier(i_ptr)
  -=
  *
  identifier(ul_ptr)
  ;
  if
  (
  identifier(i)
  !=
  -
  const int(9)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(ul)
  !=
  const unsigned long(9223372036854775807ul)
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
