-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  extern
  int
  identifier(my_fun)
  (
  void
  )
  ;
  int
  identifier(call_static_my_fun)
  (
  void
  )
  ;
  int
  identifier(call_static_my_fun_2)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(call_static_my_fun)
  (
  )
  !=
  const int(1)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(my_fun)
  (
  )
  !=
  const int(100)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(call_static_my_fun_2)
  (
  )
  !=
  const int(2)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(my_fun)
  (
  void
  )
  {
  return
  const int(100)
  ;
  }
