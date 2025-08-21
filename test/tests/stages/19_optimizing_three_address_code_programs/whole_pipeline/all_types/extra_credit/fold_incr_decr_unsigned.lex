-- Lexing ... OK
+
+
@@ Tokens @@
List[106]:
  int
  identifier(target)
  (
  void
  )
  {
  unsigned
  int
  identifier(u)
  =
  const int(0)
  ;
  unsigned
  int
  identifier(u2)
  =
  --
  identifier(u)
  ;
  unsigned
  int
  identifier(u3)
  =
  identifier(u)
  --
  ;
  unsigned
  int
  identifier(u4)
  =
  const unsigned int(4294967295U)
  ;
  unsigned
  int
  identifier(u5)
  =
  identifier(u4)
  ++
  ;
  unsigned
  int
  identifier(u6)
  =
  ++
  identifier(u4)
  ;
  if
  (
  !
  (
  identifier(u)
  ==
  const unsigned int(4294967294U)
  &&
  identifier(u2)
  ==
  const unsigned int(4294967295U)
  &&
  identifier(u3)
  ==
  const unsigned int(4294967295U)
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
  identifier(u4)
  ==
  const int(1)
  &&
  identifier(u5)
  ==
  const unsigned int(4294967295U)
  &&
  identifier(u6)
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
