-- Lexing ... OK
+
+
@@ Tokens @@
List[140]:
  int
  identifier(target_negate)
  (
  void
  )
  {
  return
  -
  const int(3)
  ;
  }
  int
  identifier(target_negate_zero)
  (
  void
  )
  {
  return
  -
  const int(0)
  ;
  }
  int
  identifier(target_not)
  (
  void
  )
  {
  return
  !
  const int(1024)
  ;
  }
  int
  identifier(target_not_zero)
  (
  void
  )
  {
  return
  !
  const int(0)
  ;
  }
  int
  identifier(target_complement)
  (
  void
  )
  {
  return
  ~
  const int(1)
  ;
  }
  int
  identifier(three)
  =
  const int(3)
  ;
  int
  identifier(two)
  =
  const int(2)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(target_negate)
  (
  )
  !=
  -
  identifier(three)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(target_negate_zero)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_not)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_not_zero)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_complement)
  (
  )
  !=
  -
  identifier(two)
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
