-- Lexing ... OK
+
+
@@ Tokens @@
List[111]:
  long
  identifier(x)
  =
  const long(5l)
  ;
  int
  identifier(add_large)
  (
  void
  )
  {
  identifier(x)
  =
  identifier(x)
  +
  const long(4294967290l)
  ;
  return
  (
  identifier(x)
  ==
  const long(4294967295l)
  )
  ;
  }
  int
  identifier(subtract_large)
  (
  void
  )
  {
  identifier(x)
  =
  identifier(x)
  -
  const long(4294967290l)
  ;
  return
  (
  identifier(x)
  ==
  const long(5l)
  )
  ;
  }
  int
  identifier(multiply_by_large)
  (
  void
  )
  {
  identifier(x)
  =
  identifier(x)
  *
  const long(4294967290l)
  ;
  return
  (
  identifier(x)
  ==
  const long(21474836450l)
  )
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
  identifier(add_large)
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
  identifier(subtract_large)
  (
  )
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  !
  identifier(multiply_by_large)
  (
  )
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
