-- Lexing ... OK
+
+
@@ Tokens @@
List[72]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  const int(0)
  ;
  if
  (
  const int(1)
  )
  {
  int
  identifier(a)
  =
  const int(5)
  ;
  goto
  identifier(other_if)
  ;
  identifier(sum)
  =
  const int(0)
  ;
  identifier(first_if)
  :
  identifier(a)
  =
  const int(5)
  ;
  identifier(sum)
  =
  identifier(sum)
  +
  identifier(a)
  ;
  }
  if
  (
  const int(0)
  )
  {
  identifier(other_if)
  :
  ;
  int
  identifier(a)
  =
  const int(6)
  ;
  identifier(sum)
  =
  identifier(sum)
  +
  identifier(a)
  ;
  goto
  identifier(first_if)
  ;
  identifier(sum)
  =
  const int(0)
  ;
  }
  return
  identifier(sum)
  ;
  }
