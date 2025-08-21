-- Lexing ... OK
+
+
@@ Tokens @@
List[136]:
  int
  identifier(i)
  =
  const int(4)
  ;
  int
  identifier(j)
  =
  const int(5)
  ;
  int
  identifier(flag_1)
  =
  const int(1)
  ;
  int
  identifier(flag_0)
  =
  const int(0)
  ;
  void
  identifier(incr_i)
  (
  void
  )
  {
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  }
  void
  identifier(incr_j)
  (
  void
  )
  {
  identifier(j)
  =
  identifier(j)
  +
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  identifier(flag_1)
  ?
  identifier(incr_i)
  (
  )
  :
  identifier(incr_j)
  (
  )
  ;
  identifier(flag_0)
  ?
  identifier(incr_i)
  (
  )
  :
  identifier(incr_j)
  (
  )
  ;
  if
  (
  identifier(i)
  !=
  const int(5)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(j)
  !=
  const int(6)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(flag_0)
  ?
  identifier(incr_j)
  (
  )
  :
  identifier(flag_1)
  ?
  identifier(incr_i)
  (
  )
  :
  identifier(incr_j)
  (
  )
  ;
  if
  (
  identifier(i)
  !=
  const int(6)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(j)
  !=
  const int(6)
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
