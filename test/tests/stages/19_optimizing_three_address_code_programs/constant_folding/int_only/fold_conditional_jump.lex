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
  const int(0)
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
  const int(1)
  &&
  const int(1)
  ;
  }
  int
  identifier(target_jnz_to_jmp)
  (
  void
  )
  {
  return
  const int(3)
  ||
  const int(99)
  ;
  }
  int
  identifier(target_remove_jnz)
  (
  void
  )
  {
  return
  const int(0)
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
