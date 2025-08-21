-- Lexing ... OK
+
+
@@ Tokens @@
List[181]:
  double
  *
  identifier(globl_ptr)
  =
  const int(0)
  ;
  void
  identifier(save_ptr)
  (
  double
  *
  identifier(to_save)
  )
  {
  identifier(globl_ptr)
  =
  identifier(to_save)
  ;
  }
  void
  identifier(update_ptr)
  (
  void
  )
  {
  *
  identifier(globl_ptr)
  =
  const double(4.0)
  ;
  }
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(x)
  =
  const int(10)
  ;
  identifier(update_ptr)
  (
  )
  ;
  return
  identifier(x)
  ;
  }
  int
  identifier(kill_aliased)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(1.0)
  ;
  double
  *
  identifier(ptr)
  =
  &
  identifier(d)
  ;
  identifier(save_ptr)
  (
  identifier(ptr)
  )
  ;
  if
  (
  *
  identifier(globl_ptr)
  !=
  const double(1.0)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(d)
  =
  const double(2.0)
  ;
  if
  (
  *
  identifier(globl_ptr)
  !=
  const double(2.0)
  )
  {
  return
  const int(0)
  ;
  }
  identifier(update_ptr)
  (
  )
  ;
  return
  identifier(d)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(0.0)
  ;
  identifier(globl_ptr)
  =
  &
  identifier(d)
  ;
  if
  (
  identifier(target)
  (
  )
  !=
  const int(10)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(4.0)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(d)
  =
  const double(0.0)
  ;
  if
  (
  identifier(kill_aliased)
  (
  )
  !=
  const double(4.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(d)
  !=
  const double(0.0)
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
