-- Lexing ... OK
+
+
@@ Tokens @@
List[89]:
  int
  identifier(target)
  (
  void
  )
  {
  char
  identifier(c1)
  =
  const int(120)
  ;
  char
  identifier(c2)
  =
  const int(3)
  ;
  char
  identifier(c3)
  =
  (
  identifier(c1)
  +
  identifier(c1)
  )
  /
  identifier(c2)
  ;
  unsigned
  char
  identifier(uc1)
  =
  const int(200)
  ;
  unsigned
  char
  identifier(uc2)
  =
  const int(12)
  ;
  unsigned
  char
  identifier(uc3)
  =
  (
  identifier(uc1)
  +
  identifier(uc1)
  )
  /
  identifier(uc2)
  ;
  if
  (
  identifier(c3)
  !=
  const int(80)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(uc3)
  !=
  const int(33)
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
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(target)
  (
  )
  ;
  }
