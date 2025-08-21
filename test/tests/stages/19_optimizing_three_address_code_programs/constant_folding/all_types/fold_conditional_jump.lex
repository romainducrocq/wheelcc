-- Lexing ... OK
+
+
@@ Tokens @@
List[110]:
  int
  identifier(target_jz_to_jmp)
  (
  void
  )
  {
  return
  const long(0l)
  &&
  const int(0)
  ;
  }
  int
  identifier(target_remove_jz)
  (
  void
  )
  {
  return
  const unsigned int(1u)
  &&
  const double(1.)
  ;
  }
  int
  identifier(target_jnz_to_jmp)
  (
  void
  )
  {
  return
  const double(3.5)
  ||
  const unsigned long(99ul)
  ;
  }
  int
  identifier(target_remove_jnz)
  (
  void
  )
  {
  return
  const unsigned long(0ul)
  ||
  const int(1)
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
  identifier(target_jz_to_jmp)
  (
  )
  !=
  const int(0)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(target_remove_jz)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(target_jnz_to_jmp)
  (
  )
  !=
  const int(1)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(target_remove_jnz)
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
  return
  const int(0)
  ;
  }
