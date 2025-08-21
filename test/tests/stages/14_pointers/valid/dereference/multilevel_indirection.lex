-- Lexing ... OK
+
+
@@ Tokens @@
List[307]:
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(10.0)
  ;
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(d)
  ;
  double
  *
  *
  identifier(d_ptr_ptr)
  =
  &
  identifier(d_ptr)
  ;
  double
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  =
  &
  identifier(d_ptr_ptr)
  ;
  if
  (
  identifier(d)
  !=
  const double(10.0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  *
  identifier(d_ptr)
  !=
  const double(10.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  *
  *
  identifier(d_ptr_ptr)
  !=
  const double(10.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  const double(10.0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  &
  identifier(d)
  !=
  identifier(d_ptr)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  *
  identifier(d_ptr_ptr)
  !=
  identifier(d_ptr)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  identifier(d_ptr)
  )
  {
  return
  const int(7)
  ;
  }
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  =
  const double(5.0)
  ;
  if
  (
  identifier(d)
  !=
  const double(5.0)
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
  const double(5.0)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  *
  *
  identifier(d_ptr_ptr)
  !=
  const double(5.0)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  const double(5.0)
  )
  {
  return
  const int(11)
  ;
  }
  double
  identifier(d2)
  =
  const double(1.0)
  ;
  double
  *
  identifier(d2_ptr)
  =
  &
  identifier(d2)
  ;
  double
  *
  identifier(d2_ptr2)
  =
  identifier(d2_ptr)
  ;
  double
  *
  *
  identifier(d2_ptr_ptr)
  =
  &
  identifier(d2_ptr)
  ;
  *
  identifier(d_ptr_ptr_ptr)
  =
  identifier(d2_ptr_ptr)
  ;
  if
  (
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  identifier(d2_ptr)
  )
  {
  return
  const int(12)
  ;
  }
  if
  (
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  const double(1.0)
  )
  {
  return
  const int(13)
  ;
  }
  if
  (
  identifier(d2_ptr_ptr)
  ==
  &
  identifier(d2_ptr2)
  )
  return
  const int(14)
  ;
  identifier(d2_ptr)
  =
  identifier(d_ptr)
  ;
  if
  (
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  identifier(d_ptr)
  )
  {
  return
  const int(15)
  ;
  }
  if
  (
  *
  identifier(d2_ptr_ptr)
  !=
  identifier(d_ptr)
  )
  {
  return
  const int(16)
  ;
  }
  if
  (
  *
  *
  identifier(d_ptr_ptr_ptr)
  ==
  identifier(d2_ptr2)
  )
  {
  return
  const int(17)
  ;
  }
  if
  (
  *
  *
  *
  identifier(d_ptr_ptr_ptr)
  !=
  const double(5.0)
  )
  {
  return
  const int(18)
  ;
  }
  return
  const int(0)
  ;
  }
