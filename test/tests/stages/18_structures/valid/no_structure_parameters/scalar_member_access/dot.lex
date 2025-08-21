-- Lexing ... OK
+
+
@@ Tokens @@
List[507]:
  struct
  identifier(four_members)
  {
  double
  identifier(d)
  ;
  char
  identifier(c)
  ;
  long
  identifier(l)
  ;
  char
  *
  identifier(ptr)
  ;
  }
  ;
  double
  identifier(get_double)
  (
  void
  )
  {
  return
  const double(2e12)
  ;
  }
  static
  long
  identifier(l)
  =
  const long(34359738378l)
  ;
  int
  identifier(accept_params)
  (
  int
  identifier(d_divided)
  ,
  int
  identifier(c_doubled)
  ,
  double
  identifier(l_cast)
  ,
  int
  identifier(dereferenced_ptr)
  ,
  double
  identifier(d)
  ,
  int
  identifier(c)
  ,
  long
  identifier(l)
  ,
  char
  *
  identifier(ptr)
  )
  {
  if
  (
  identifier(d)
  !=
  const double(4e12)
  ||
  identifier(c)
  !=
  const int(127)
  ||
  identifier(l)
  !=
  const long(8589934594l)
  ||
  *
  identifier(ptr)
  !=
  const int(100)
  ||
  identifier(d_divided)
  !=
  const double(100.0)
  ||
  identifier(c_doubled)
  !=
  const int(254)
  ||
  identifier(l_cast)
  !=
  const double(8589934594.0)
  ||
  identifier(dereferenced_ptr)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_auto)
  (
  void
  )
  {
  struct
  identifier(four_members)
  identifier(autom)
  ;
  identifier(autom)
  .
  identifier(d)
  =
  identifier(get_double)
  (
  )
  *
  const double(2.0)
  ;
  identifier(autom)
  .
  identifier(c)
  =
  const int(127)
  ;
  identifier(autom)
  .
  identifier(l)
  =
  identifier(l)
  /
  const int(4)
  ;
  char
  identifier(chr)
  =
  const int(100)
  ;
  identifier(autom)
  .
  identifier(ptr)
  =
  &
  identifier(chr)
  ;
  if
  (
  identifier(autom)
  .
  identifier(d)
  !=
  const double(4e12)
  ||
  identifier(autom)
  .
  identifier(c)
  !=
  const int(127)
  ||
  identifier(autom)
  .
  identifier(l)
  !=
  const long(8589934594l)
  ||
  identifier(autom)
  .
  identifier(ptr)
  !=
  &
  identifier(chr)
  )
  {
  return
  const int(0)
  ;
  }
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(autom)
  .
  identifier(d)
  ;
  char
  *
  identifier(c_ptr)
  =
  &
  identifier(autom)
  .
  identifier(c)
  ;
  if
  (
  *
  identifier(d_ptr)
  !=
  const double(4e12)
  ||
  *
  identifier(c_ptr)
  !=
  const int(127)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  *
  identifier(autom)
  .
  identifier(ptr)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  !
  identifier(accept_params)
  (
  identifier(autom)
  .
  identifier(d)
  /
  const double(4e10)
  ,
  identifier(autom)
  .
  identifier(c)
  *
  const int(2)
  ,
  (
  double
  )
  identifier(autom)
  .
  identifier(l)
  ,
  *
  identifier(autom)
  .
  identifier(ptr)
  ,
  identifier(autom)
  .
  identifier(d)
  ,
  identifier(autom)
  .
  identifier(c)
  ,
  identifier(autom)
  .
  identifier(l)
  ,
  identifier(autom)
  .
  identifier(ptr)
  )
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(test_static)
  (
  void
  )
  {
  static
  struct
  identifier(four_members)
  identifier(stat)
  ;
  static
  char
  identifier(chr)
  =
  const int(100)
  ;
  identifier(stat)
  .
  identifier(d)
  =
  identifier(get_double)
  (
  )
  *
  const double(2.0)
  ;
  identifier(stat)
  .
  identifier(c)
  =
  const int(127)
  ;
  identifier(stat)
  .
  identifier(l)
  =
  identifier(l)
  /
  const int(4)
  ;
  identifier(stat)
  .
  identifier(ptr)
  =
  &
  identifier(chr)
  ;
  if
  (
  identifier(stat)
  .
  identifier(d)
  !=
  const double(4e12)
  ||
  identifier(stat)
  .
  identifier(c)
  !=
  const int(127)
  ||
  identifier(stat)
  .
  identifier(l)
  !=
  const long(8589934594l)
  ||
  identifier(stat)
  .
  identifier(ptr)
  !=
  &
  identifier(chr)
  )
  {
  return
  const int(0)
  ;
  }
  double
  *
  identifier(d_ptr)
  =
  &
  identifier(stat)
  .
  identifier(d)
  ;
  char
  *
  identifier(c_ptr)
  =
  &
  identifier(stat)
  .
  identifier(c)
  ;
  if
  (
  *
  identifier(d_ptr)
  !=
  const double(4e12)
  ||
  *
  identifier(c_ptr)
  !=
  const int(127)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  *
  identifier(stat)
  .
  identifier(ptr)
  !=
  const int(100)
  )
  {
  return
  const int(0)
  ;
  }
  if
  (
  !
  identifier(accept_params)
  (
  identifier(stat)
  .
  identifier(d)
  /
  const double(4e10)
  ,
  identifier(stat)
  .
  identifier(c)
  *
  const int(2)
  ,
  (
  double
  )
  identifier(stat)
  .
  identifier(l)
  ,
  *
  identifier(stat)
  .
  identifier(ptr)
  ,
  identifier(stat)
  .
  identifier(d)
  ,
  identifier(stat)
  .
  identifier(c)
  ,
  identifier(stat)
  .
  identifier(l)
  ,
  identifier(stat)
  .
  identifier(ptr)
  )
  )
  {
  return
  const int(0)
  ;
  }
  return
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  !
  identifier(test_auto)
  (
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  !
  identifier(test_static)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
