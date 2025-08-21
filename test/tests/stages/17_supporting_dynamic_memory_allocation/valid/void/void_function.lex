-- Lexing ... OK
+
+
@@ Tokens @@
List[81]:
  int
  identifier(foo)
  =
  const int(0)
  ;
  void
  identifier(set_foo_to_positive_num)
  (
  int
  identifier(a)
  )
  {
  if
  (
  identifier(a)
  <
  const int(0)
  )
  {
  return
  ;
  }
  identifier(foo)
  =
  identifier(a)
  ;
  return
  ;
  }
  void
  identifier(do_nothing)
  (
  void
  )
  {
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(set_foo_to_positive_num)
  (
  -
  const int(2)
  )
  ;
  if
  (
  identifier(foo)
  )
  {
  return
  const int(1)
  ;
  }
  identifier(set_foo_to_positive_num)
  (
  const int(12)
  )
  ;
  if
  (
  identifier(foo)
  !=
  const int(12)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(do_nothing)
  (
  )
  ;
  return
  const int(0)
  ;
  }
