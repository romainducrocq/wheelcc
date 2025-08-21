-- Lexing ... OK
+
+
@@ Tokens @@
List[205]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(i)
  =
  -
  const int(100)
  ;
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(13835058055282163712ul)
  ;
  double
  identifier(d)
  =
  const double(3.5)
  ;
  int
  *
  identifier(i_ptr)
  =
  &
  identifier(i)
  ;
  unsigned
  long
  *
  identifier(ul_ptr)
  =
  &
  identifier(ul)
  ;
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  if
  (
  *
  identifier(i_ptr)
  !=
  -
  const int(100)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  identifier(ul_ptr)
  !=
  const unsigned long(13835058055282163712ul)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  *
  identifier(d_ptr)
  !=
  const double(3.5)
  )
  {
  return
  const int(3)
  ;
  }
  identifier(i)
  =
  const int(12)
  ;
  identifier(ul)
  =
  const int(1000)
  ;
  identifier(d)
  =
  -
  const double(000.001)
  ;
  if
  (
  *
  identifier(i_ptr)
  !=
  const int(12)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  *
  identifier(ul_ptr)
  !=
  const int(1000)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  *
  identifier(d_ptr)
  !=
  -
  const double(000.001)
  )
  {
  return
  const int(6)
  ;
  }
  int
  identifier(i2)
  =
  const int(1)
  ;
  unsigned
  long
  identifier(ul2)
  =
  const unsigned long(144115196665790464ul)
  ;
  double
  identifier(d2)
  =
  -
  const double(33.3)
  ;
  identifier(i_ptr)
  =
  &
  identifier(i2)
  ;
  identifier(ul_ptr)
  =
  &
  identifier(ul2)
  ;
  identifier(d_ptr)
  =
  &
  identifier(d2)
  ;
  if
  (
  *
  identifier(i_ptr)
  !=
  const int(1)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  *
  identifier(ul_ptr)
  !=
  const unsigned long(144115196665790464ul)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  *
  identifier(d_ptr)
  !=
  -
  const double(33.3)
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
