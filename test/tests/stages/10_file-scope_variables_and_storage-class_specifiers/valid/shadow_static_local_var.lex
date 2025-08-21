-- Lexing ... OK
+
+
@@ Tokens @@
List[141]:
  int
  identifier(i)
  ;
  int
  identifier(update_static_or_global)
  (
  int
  identifier(update_global)
  ,
  int
  identifier(new_val)
  )
  {
  static
  int
  identifier(i)
  ;
  if
  (
  identifier(update_global)
  )
  {
  extern
  int
  identifier(i)
  ;
  identifier(i)
  =
  identifier(new_val)
  ;
  }
  else
  identifier(i)
  =
  identifier(new_val)
  ;
  return
  identifier(i)
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
  identifier(i)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  int
  identifier(result)
  =
  identifier(update_static_or_global)
  (
  const int(1)
  ,
  const int(10)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(0)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
  )
  return
  const int(1)
  ;
  identifier(result)
  =
  identifier(update_static_or_global)
  (
  const int(0)
  ,
  const int(9)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(9)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
  )
  return
  const int(1)
  ;
  identifier(result)
  =
  identifier(update_static_or_global)
  (
  const int(1)
  ,
  const int(11)
  )
  ;
  if
  (
  identifier(result)
  !=
  const int(9)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(i)
  !=
  const int(11)
  )
  return
  const int(1)
  ;
  return
  const int(0)
  ;
  }
