-- Lexing ... OK
+
+
@@ Tokens @@
List[155]:
  int
  identifier(check_aligment)
  (
  char
  *
  identifier(c)
  )
  {
  unsigned
  long
  identifier(l)
  =
  (
  unsigned
  long
  )
  identifier(c)
  ;
  return
  (
  identifier(l)
  %
  const int(16)
  ==
  const int(0)
  )
  ;
  }
  static
  signed
  char
  identifier(flat_static)
  [
  const int(16)
  ]
  =
  string literal("x")
  ;
  static
  unsigned
  char
  identifier(nested_static)
  [
  const int(3)
  ]
  [
  const int(4)
  ]
  [
  const int(2)
  ]
  =
  {
  {
  string literal("a")
  }
  ,
  {
  string literal("b")
  }
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  identifier(flat_auto)
  [
  const int(22)
  ]
  ;
  char
  identifier(nested_auto)
  [
  const int(10)
  ]
  [
  const int(3)
  ]
  ;
  if
  (
  !
  identifier(check_aligment)
  (
  (
  char
  *
  )
  identifier(flat_static)
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
  identifier(check_aligment)
  (
  (
  char
  *
  )
  identifier(nested_static)
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
  identifier(check_aligment)
  (
  (
  char
  *
  )
  identifier(flat_auto)
  )
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  !
  identifier(check_aligment)
  (
  (
  char
  *
  )
  identifier(nested_auto)
  )
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
