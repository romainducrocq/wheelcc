-- Lexing ... OK
+
+
@@ Tokens @@
List[109]:
  int
  identifier(target)
  (
  void
  )
  {
  signed
  char
  identifier(s)
  =
  -
  const int(127)
  ;
  signed
  char
  identifier(s2)
  =
  --
  identifier(s)
  ;
  signed
  char
  identifier(s3)
  =
  identifier(s)
  --
  ;
  unsigned
  char
  identifier(uc1)
  =
  const int(255)
  ;
  unsigned
  char
  identifier(uc2)
  =
  identifier(uc1)
  ++
  ;
  unsigned
  char
  identifier(uc3)
  =
  ++
  identifier(uc1)
  ;
  if
  (
  !
  (
  identifier(s)
  ==
  const int(127)
  &&
  identifier(s2)
  ==
  -
  const int(128)
  &&
  identifier(s3)
  ==
  -
  const int(128)
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
  (
  identifier(uc1)
  ==
  const int(1)
  &&
  identifier(uc2)
  ==
  const int(255)
  &&
  identifier(uc3)
  ==
  const int(1)
  )
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
