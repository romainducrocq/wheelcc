-- Lexing ... OK
+
+
@@ Tokens @@
List[135]:
  long
  identifier(target_and)
  (
  void
  )
  {
  return
  const long(1085102592571150095l)
  &
  const long(71777214294589695l)
  ;
  }
  long
  identifier(target_or)
  (
  void
  )
  {
  return
  const long(1085102592571150095l)
  |
  const long(71777214294589695l)
  ;
  }
  long
  identifier(target_xor)
  (
  void
  )
  {
  return
  const long(1085102592571150095l)
  ^
  const long(71777214294589695l)
  ;
  }
  long
  identifier(target_shift_left)
  (
  void
  )
  {
  return
  const long(1l)
  <<
  const int(62)
  ;
  }
  long
  identifier(target_shift_right)
  (
  void
  )
  {
  return
  const long(72057589742960640l)
  >>
  const int(35)
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
  identifier(target_and)
  (
  )
  !=
  const long(4222189076152335l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_or)
  (
  )
  !=
  const int(1152657617789587455)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_xor)
  (
  )
  !=
  const long(1148435428713435120l)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_shift_left)
  (
  )
  !=
  const long(4611686018427387904l)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(target_shift_right)
  (
  )
  !=
  const int(2097151)
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
